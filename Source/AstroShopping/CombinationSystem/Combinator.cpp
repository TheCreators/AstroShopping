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

    MaxProgress = 9;
	Progress = 0;
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
	DOREPLIFETIME(ACombinator, ProductModelUrls);
	DOREPLIFETIME(ACombinator, ProductThumbnailUrls);
	DOREPLIFETIME(ACombinator, bIsCombining);
	DOREPLIFETIME(ACombinator, Progress);
	DOREPLIFETIME(ACombinator, ProductName);
	DOREPLIFETIME(ACombinator, Product);
}

void ACombinator::Reset()
{
	ProductThumbnails.Empty();
    ProductThumbnails.SetNumZeroed(4);

	bIsProductSelected = false;

	LocalProduct = nullptr;

	ProductMesh = nullptr;

	SelectedProductIndex = -1;
	
	if (GetLocalRole() == ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Combinator reset from Server"));

		bIsCombining = false;

        Progress = 0;
		OnRep_Progress();

		ProductName.Reset();
		OnRep_ProductName();

		ProductModelUrls.Empty();
        ProductModelUrls.SetNumZeroed(4);

		ProductThumbnailUrls.Empty();
        ProductThumbnailUrls.SetNumZeroed(4);
        OnRep_ProductThumbnailUrls();

		Product = nullptr;
	}

    OnCombinatorReset.Broadcast();
}

void ACombinator::IncreaseProgress()
{
    Progress++;
    OnRep_Progress();
}

void ACombinator::ResetProgress()
{
    Progress = 0;
    OnRep_Progress();
}

void ACombinator::Server_StartCombination_Implementation(const FString& FirstProductName, const FString& SecondProductName)
{
    bIsCombining = true;

    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Combining %s & %s"), *FirstProductName, *SecondProductName));

    FString Prompt = FString::Printf(TEXT(R"(Combine %s & %s into a new item. Return valid JSON:{"name": "Simple Combined Name","desc": "Clear, literal description for 3D modeling. Focus on basic shape and material. Max 15 words"}. Avoid metaphors and poetic language)"), *FirstProductName, *SecondProductName);

    GenerateCombinedItemProps(
        Prompt,
        [this](FString GeneratedName, FString GeneratedDescription) {
            ProductName = GeneratedName;
            OnRep_ProductName();
            IncreaseProgress();

            GenerateCombinedItemModels(
                GeneratedDescription,
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
			}

			if (bHasModelUrl && ProductModelUrls[ModelIndex].IsEmpty())
			{
				ProductModelUrls[ModelIndex] = StatusResponse.ModelUrl;
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

void ACombinator::OnRep_ProductThumbnailUrls()
{
	if (!bIsCombining)
	{
		return;
	}

	for (int32 i = 0; i < ProductThumbnailUrls.Num(); i++)
	{
		if (ProductThumbnailUrls[i].IsEmpty() || ProductThumbnails[i] != nullptr)
		{
			continue;
		}

		ProductThumbnails[i] = UTexture2D::CreateTransient(1, 1);
		OnProductThumbnailLoaded.Broadcast(i);

        if (GetLocalRole() == ROLE_Authority)
        {
            IncreaseProgress();
        }
	}
}

void ACombinator::Server_RequestLoadProduct_Implementation(int32 Index)
{
    IncreaseProgress();
    Multicast_LoadProduct(Index);
}

void ACombinator::Multicast_LoadProduct_Implementation(int32 Index)
{
    SelectedProductIndex = Index;
	bIsProductSelected = true;
    TryLoadProduct();
}

void ACombinator::TryLoadProduct()
{
    if (ProductModelUrls[SelectedProductIndex].IsEmpty())
    {
        if (!GetWorld())
        {
            UE_LOG(LogTemp, Error, TEXT("World context is missing for product loading."));
            return;
        }

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &ACombinator::TryLoadProduct,
            0.1f,
            false
        );

        return;
    }

    TMap<FString, FString> Headers;
    FglTFRuntimeConfig LoaderConfig;
    FglTFRuntimeHttpResponse Delegate;
    Delegate.BindDynamic(this, &ACombinator::OnModelDownloaded);

    UglTFRuntimeFunctionLibrary::glTFLoadAssetFromUrl(
        ProductModelUrls[SelectedProductIndex],
        Headers,
        Delegate,
        LoaderConfig
    );

	if (GetLocalRole() == ROLE_Authority)
	{
		IncreaseProgress();
	}
}

void ACombinator::OnModelDownloaded(UglTFRuntimeAsset* Asset)
{
    if (Asset == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load model asset"));
        return;
    }

    if (GetLocalRole() == ROLE_Authority)
    {
        IncreaseProgress();
    }

    FglTFRuntimeStaticMeshConfig StaticMeshConfig;
    StaticMeshConfig.bBuildSimpleCollision = true;

    FglTFRuntimeStaticMeshAsync AsyncCallback;
	AsyncCallback.BindDynamic(this, &ACombinator::OnModelLoaded);
	Asset->LoadStaticMeshAsync(0, AsyncCallback, StaticMeshConfig);
}

void ACombinator::OnModelLoaded(UStaticMesh* Mesh)
{
	if (Mesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load model"));
		return;
	}

	ProductMesh = Mesh;

    if (GetLocalRole() != ROLE_Authority)
    {
		TrySetProductMesh();
        return;
    }

    IncreaseProgress();

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

	Product = GameMode->GetProductManager()->SpawnProduct(
        GetProductSpawnPoint(),
		FGuid::NewGuid(),
		ProductName,
        ProductMesh,
		ProductThumbnails[SelectedProductIndex]
    );
	Product->Mesh->SetStaticMesh(ProductMesh);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        this,
        &ACombinator::Reset,
        0.5f,
        false
    );
}

void ACombinator::OnRep_Product()
{
    if (Product == nullptr)
    {
        return;
    }

    LocalProduct = Product;
}

void ACombinator::TrySetProductMesh()
{
	if (LocalProduct == nullptr)
	{
		if (!GetWorld())
		{
			UE_LOG(LogTemp, Error, TEXT("World context is missing for product mesh setting."));
			return;
		}
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&ACombinator::TrySetProductMesh,
			0.1f,
			false
		);
		return;
	}

	LocalProduct->ProductThumbnail = ProductThumbnails[SelectedProductIndex];
	LocalProduct->ProductMesh = ProductMesh;
	LocalProduct->Mesh->SetStaticMesh(ProductMesh);

    Reset();
}