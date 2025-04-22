#include "Combinator.h"
#include "Net/UnrealNetwork.h"
#include <ctime>
#include "glTFRuntimeFunctionLibrary.h"
#include "AstroShopping/AI/LlmApiClient.h"
#include "AstroShopping/AI/GenieApiClient.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "AstroShopping/AstroShoppingUserSettings.h"
#include "AstroShopping/ProductSystem/Product.h"
#include "AstroShopping/ProductSystem/ProductManager.h"
#include "AstroShopping/GameMode/GameGameMode.h"

ACombinator::ACombinator()
{
    srand(static_cast<unsigned int>(time(0)));

    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    MaxProgress = 8;
    MaxPollingAttempts = 32;
    PollDelaySeconds = 2.0f;
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

    Reset();
}

void ACombinator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACombinator, Progress);
    DOREPLIFETIME(ACombinator, ProductName);
    DOREPLIFETIME(ACombinator, SelectedProductIndex);
    DOREPLIFETIME(ACombinator, ProductThumbnailUrls);
}

void ACombinator::Reset()
{
    ProductModelUrls.Empty();
    ProductModelUrls.SetNumZeroed(4);

    SelectedProductIndex = -1;
	OnRep_SelectedProductIndex();

    Progress = 0;
    OnRep_Progress();

    ProductName.Reset();
    OnRep_ProductName();

    ProductThumbnailUrls.Empty();
    ProductThumbnailUrls.SetNumZeroed(4);
    OnRep_ProductThumbnailUrls();

    OnReset();
}

bool ACombinator::IsWorking() const
{
    return Progress > 0;
}

void ACombinator::IncreaseProgress()
{
    Progress++;
    OnRep_Progress();
}

void ACombinator::Server_StartCombination_Implementation(
    const FString& FirstProductName,
    const FString& SecondProductName,
    const int32 FirstProductPrice,
    const int32 SecondProductPrice
)
{
    IncreaseProgress();

    UE_LOG(LogTemp, Display, TEXT("Combining %s & %s"), *FirstProductName, *SecondProductName);

	FString Prompt = FString::Printf(TEXT(R"(Combine %s (price %s) & %s (price %s) into a new item; generate an interesting game price for the combined item, considering the input prices - the result could be more or less valuable than the sum of its parts reflecting game balance or crafting synergy; return valid JSON:{"name": "Simple Combined Name", "desc": "Clear, literal description for 3D modeling. Focus on basic shape and material. Max 15 words", "price": int} Avoid metaphors and poetic language.)"), *FirstProductName, *FString::FromInt(FirstProductPrice), *SecondProductName, *FString::FromInt(SecondProductPrice));

    GenerateCombinedItemProps(
        Prompt,
        [this](FString GeneratedName, FString GeneratedDescription, int32 GeneratedPrice) {
            ProductName = GeneratedName;
            OnRep_ProductName();

			ProductPrice = GeneratedPrice;

            IncreaseProgress();

            GenerateCombinedItemModels(
                GeneratedDescription,
                [](FString ErrorMessage) {
                    UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
                }
            );
        },
        [](FString ErrorMessage) {
            UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
        }
    );
}

void ACombinator::GenerateCombinedItemProps(
    const FString& InputText,
    TFunction<void(FString, FString, int32)> OnSuccess,
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
				int32 Price;
                if (DataObject->TryGetStringField(TEXT("name"), Name) &&
                    DataObject->TryGetStringField(TEXT("desc"), Description) &&
					DataObject->TryGetNumberField(TEXT("price"), Price)
                )
                {
                    OnSuccess(Name, Description, Price);
                }
                else
                {
                    OnError(TEXT("Unable to extract 'name' or 'description' or 'price' field from JSON. Response: ") + ResponseContent);
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

void ACombinator::GenerateCombinedItemModels(const FString& Prompt, TFunction<void(FString)> OnError)
{
    if (!GenieApiClient)
    {
        OnError(TEXT("GenieApiClient is not initialized"));
        return;
    }

    GenieApiClient->SendModelsGenerationRequest(
        Prompt,
        [this, OnError](const FModelsGenerationResponse& Response)
        {
            if (Response.ModelIds.Num() == 0)
            {
                OnError(TEXT("No model IDs were returned."));
                return;
            }

            for (int32 i = 0; i < Response.ModelIds.Num(); i++)
            {
                PollModelStatus(Response.ModelIds[i], MaxPollingAttempts, i, OnError);
            }
        },
        OnError
    );
}

void ACombinator::PollModelStatus(const FString& ModelId, int32 AttemptsRemaining, int32 ModelIndex, TFunction<void(FString)> OnError)
{
    if (!IsWorking())
	{
		return;
	}

    if (AttemptsRemaining <= 0)
    {
        OnError(TEXT("Exceeded maximum polling attempts."));
        return;
    }

    GenieApiClient->SendModelsStatusRequest(
        ModelId,
        [this, ModelId, AttemptsRemaining, ModelIndex, OnError](const FModelStatusResponse& StatusResponse)
        {
            bool bHasThumbnailUrl = !StatusResponse.ThumbnailUrl.IsEmpty();
            bool bHasModelUrl = !StatusResponse.ModelUrl.IsEmpty();

            if (bHasThumbnailUrl && ProductThumbnailUrls[ModelIndex].IsEmpty())
            {
                ProductThumbnailUrls[ModelIndex] = StatusResponse.ThumbnailUrl;
                OnRep_ProductThumbnailUrls();
                IncreaseProgress();
            }

            if (bHasModelUrl && ProductModelUrls[ModelIndex].IsEmpty())
            {
                ProductModelUrls[ModelIndex] = StatusResponse.ModelUrl;
                if (SelectedProductIndex == ModelIndex)
                {
                    GotSelectedProductModelUrl.ExecuteIfBound();
                }
            }

            if (bHasThumbnailUrl && bHasModelUrl)
            {
                return;
            }

            if (!GetWorld())
            {
                OnError(TEXT("World context is missing for status polling."));
                return;
            }

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle,
                FTimerDelegate::CreateLambda([this, ModelId, AttemptsRemaining, ModelIndex, OnError]()
                    {
                        PollModelStatus(ModelId, AttemptsRemaining - 1, ModelIndex, OnError);
                    }),
                PollDelaySeconds,
                false
            );
        },
        OnError
    );
}

void ACombinator::Server_RequestLoadProduct_Implementation(int32 Index)
{
    IncreaseProgress();

    SelectedProductIndex = Index;
	OnRep_SelectedProductIndex();

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("World context is missing for product loading."));
        return;
    }

    AGameGameMode* GameMode = Cast<AGameGameMode>(GetWorld()->GetAuthGameMode());

    if (GameMode == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast GameMode"));
        return;
    }

    GotSelectedProductModelUrl.BindLambda([this, GameMode]()
        {
            IncreaseProgress();

            GameMode->GetProductManager()->RequestNewProductSpawn(
                { ProductName, ProductPrice, ProductModelUrls[SelectedProductIndex] },
                FGuid::NewGuid(),
                GetProductSpawnPoint(),
                10
            );

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle,
                this,
                &ACombinator::Reset,
                5.0f,
                false
            );
        });

    if (!ProductModelUrls[SelectedProductIndex].IsEmpty())
    {
		GotSelectedProductModelUrl.ExecuteIfBound();
    }
}
