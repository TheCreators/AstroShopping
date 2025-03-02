#include "Combinator.h"
#include "Net/UnrealNetwork.h"
#include <ctime>
#include "glTFRuntimeFunctionLibrary.h"
#include "AstroShopping/AI/LlmApiClient.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

ACombinator::ACombinator()
{
	srand(static_cast<unsigned int>(time(0)));

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

    LlmApiClient = MakeUnique<FLlmApiClient>(
        FPlatformMisc::GetEnvironmentVariable(TEXT("ASTRO_SHOPPING_LLM_API_KEY")),
        TEXT("https://api.proxyapi.ru/google/v1/models/gemini-2.0-flash:generateContent")
    );
}

void ACombinator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACombinator, ModelUrls);
	DOREPLIFETIME(ACombinator, bIsCombining);
}

void ACombinator::GenerateCombinedItemProps(
    const FString& InputText,
    TFunction<void(FString, FString)> OnSuccess,
    TFunction<void(FString)> OnError)
{
    if (!LlmApiClient)
    {
        OnError(TEXT("LlmApiClient is not initialized"));
        return;
    }

    LlmApiClient->SendRequest(InputText,
        [this, OnSuccess, OnError](const FString& ResponseContent)
        {
            FString CleanedJson = ResponseContent;
            CleanedJson = CleanedJson.Replace(TEXT("```json"), TEXT(""));
            CleanedJson = CleanedJson.Replace(TEXT("```"), TEXT(""));
            CleanedJson = CleanedJson.TrimStartAndEnd();

            TSharedPtr<FJsonObject> DataObject;
            TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(CleanedJson);
            if (FJsonSerializer::Deserialize(JsonReader, DataObject) && DataObject.IsValid())
            {
                FString Name, Description;
                if (DataObject->TryGetStringField(TEXT("name"), Name) &&
                    DataObject->TryGetStringField(TEXT("desc"), Description))
                {
                    OnSuccess(Name, Description);
                }
                else
                {
                    OnError(TEXT("Unable to extract 'name' or 'description' field from JSON. Response: ") + ResponseContent);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse cleaned JSON: %s"), *CleanedJson);
                OnError(TEXT("Failed to parse cleaned JSON."));
            }
        },
        OnError,
        true
    );
}


void ACombinator::Server_StartCombination_Implementation(const FString& FirstProductName, const FString& SecondProductName)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Combining %s & %s"), *FirstProductName, *SecondProductName));

    FString Prompt = FString::Printf(TEXT(R"(Combine %s & %s into a new item. Return valid JSON:{"name": "Title Case Name","desc": "Concise, vivid description for 3D modeling. Max 20 words"}. Ensure description is clear for visual representation)"), *FirstProductName, *SecondProductName);

    GenerateCombinedItemProps(
        Prompt,
        [this](FString GeneratedName, FString GeneratedDescription) {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Generated Name: %s"), *GeneratedName));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Generated Description: %s"), *GeneratedDescription));
        },
        [](FString ErrorMessage) {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, ErrorMessage);
        }
    );

	return;

	bIsCombining = true;

	AsyncTask(ENamedThreads::AnyThread, [this]()
		{
			//FPlatformProcess::Sleep(3.0);

			TArray<FString> NewModelUrls = {
				"https://cdn-luma.com/blender_convert/c6bb0fc8-8dab-4296-9663-962541b1bd98/cf23404dac6a_a08730d30c54_a_diamond_blue_Crys.glb",
				"https://cdn-luma.com/imagine_3d_one/fee121c7-63b9-4b2f-91cd-a33218a21ac1/18119caf96ee_Test_sign__3d_asset_0_glb.glb",
				"https://cdn-luma.com/imagine_3d_one/4997791a-315c-4d24-ba68-40da419010b8/91fc50713cdb_Test_sign__3d_asset_0_glb.glb"
			};

			int RandomIndex = rand() % NewModelUrls.Num();

			FString Url = NewModelUrls[RandomIndex];

			AsyncTask(ENamedThreads::GameThread, [this, Url]()
				{
					ModelUrls.Add(Url);
					OnRep_ModelUrls();
				});
		});
}

void ACombinator::OnRep_ModelUrls()
{
	FString ModelUrl = ModelUrls.Last();

	TMap<FString, FString> Headers;
	FglTFRuntimeConfig LoaderConfig;
	FglTFRuntimeHttpResponse ResponseDelegate;
	ResponseDelegate.BindDynamic(this, &ACombinator::OnModelDownloaded);

	UglTFRuntimeFunctionLibrary::glTFLoadAssetFromUrl(ModelUrl, Headers, ResponseDelegate, LoaderConfig);
}

void ACombinator::OnModelDownloaded(UglTFRuntimeAsset* Asset)
{
	if (Asset == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load model asset"));
		return;
	}

	FglTFRuntimeStaticMeshConfig StaticMeshConfig;
	StaticMeshConfig.bBuildSimpleCollision = true;

	FglTFRuntimeStaticMeshAsync Delegate;
	Delegate.BindDynamic(this, &ACombinator::OnModelLoaded);

	Asset->LoadStaticMeshAsync(0, Delegate, StaticMeshConfig);
}

void ACombinator::OnModelLoaded(UStaticMesh* StaticMesh)
{
	OnCombinationComplete.Broadcast(StaticMesh);
	bIsCombining = false;
}
