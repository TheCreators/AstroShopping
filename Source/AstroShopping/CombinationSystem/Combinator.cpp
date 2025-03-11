#include "Combinator.h"
#include "Net/UnrealNetwork.h"
#include <ctime>
#include "glTFRuntimeFunctionLibrary.h"
#include "AstroShopping/AI/LlmApiClient.h"
#include "AstroShopping/AI/GenieApiClient.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "AstroShopping/AstroShoppingUserSettings.h"

ACombinator::ACombinator()
{
	srand(static_cast<unsigned int>(time(0)));

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ACombinator::BeginPlay()
{
	Super::BeginPlay();

	UAstroShoppingUserSettings* UserSettings = UAstroShoppingUserSettings::GetAstroShoppingUserSettings();

	LlmApiClient = MakeUnique<FLlmApiClient>(
		UserSettings->GetProxyApiKey(),
		TEXT("https://api.proxyapi.ru/google/v1/models/gemini-2.0-flash:generateContent")
	);

	GenieApiClient = MakeUnique<FGenieApiClient>(
		UserSettings->GetGenieRefreshToken()
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

void ACombinator::GenerateCombinedItemModels(const FString& Prompt, TFunction<void(FString)> OnSuccess, TFunction<void(FString)> OnError)
{
    if (!GenieApiClient)
    {
        OnError(TEXT("GenieApiClient is not initialized"));
        return;
    }

    // Send the initial request to generate models.
    GenieApiClient->SendModelsGenerationRequest(
        Prompt,
        [this, OnSuccess, OnError](const FModelsGenerationResponse& ModelsGenerationResponse)
        {
            if (ModelsGenerationResponse.ModelIds.Num() == 0)
            {
                OnError(TEXT("No model IDs were returned."));
                return;
            }

            FString ModelId = ModelsGenerationResponse.ModelIds[0];

            TSharedPtr<TFunction<void()>> PollStatusPtr = MakeShared<TFunction<void()>>();
            *PollStatusPtr = [this, ModelId, OnSuccess, OnError, PollStatusPtr]()
                {
                    GenieApiClient->SendModelsStatusRequest(
                        ModelId,
                        [this, ModelId, OnSuccess, OnError, PollStatusPtr](const FModelStatusResponse& ModelStatusResponse)
                        {
                            UE_LOG(LogTemp, Log, TEXT("Polled status for model %s: %s. Model url: %s"), *ModelId, *ModelStatusResponse.Status, *ModelStatusResponse.ThumbnailUrl);

                            //if (ModelStatusResponse.Status.Equals(TEXT("completed"), ESearchCase::IgnoreCase))
                            if (!ModelStatusResponse.ModelUrl.Equals(TEXT("")))
                            {
                                OnSuccess(ModelStatusResponse.ModelUrl);
                            }
                            else
                            {
                                const float PollDelay = 2.0f;
                                if (GetWorld())
                                {
                                    FTimerHandle TimerHandle;
                                    GetWorld()->GetTimerManager().SetTimer(
                                        TimerHandle,
                                        FTimerDelegate::CreateLambda([PollStatusPtr]()
                                            {
                                                (*PollStatusPtr)();
                                            }),
                                        PollDelay,
                                        false
                                    );
                                }
                                else
                                {
                                    OnError(TEXT("World context is missing for status polling."));
                                }
                            }
                        },
                        OnError
                    );
                };

            // Start polling.
            (*PollStatusPtr)();
        },
        OnError
    );
}

void ACombinator::Server_StartCombination_Implementation(const FString& FirstProductName, const FString& SecondProductName)
{
    bIsCombining = true;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Combining %s & %s"), *FirstProductName, *SecondProductName));

    FString Prompt = FString::Printf(TEXT(R"(Combine %s & %s into a new item. Return valid JSON:{"name": "Simple Combined Name","desc": "Clear, literal description for 3D modeling. Focus on basic shape and material. Max 15 words"}. Avoid metaphors and poetic language)"), *FirstProductName, *SecondProductName);

    GenerateCombinedItemProps(
        Prompt,
        [this](FString GeneratedName, FString GeneratedDescription) {
            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Generated Name: %s"), *GeneratedName));
            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Generated Description: %s"), *GeneratedDescription));

            GenerateCombinedItemModels(
                GeneratedDescription,
                [this](FString ModelUrl) {
                    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Model URL: %s"), *ModelUrl));
					AsyncTask(ENamedThreads::AnyThread, [this, ModelUrl]()
                        {
							AsyncTask(ENamedThreads::GameThread, [this, ModelUrl]()
                                {
                                    ModelUrls.Add(ModelUrl);
                                    OnRep_ModelUrls();
                                });
                        });
                },
                [](FString ErrorMessage) {
                    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, ErrorMessage);
                }
            );
        },
        [](FString ErrorMessage) {
            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, ErrorMessage);
        }
    );
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
