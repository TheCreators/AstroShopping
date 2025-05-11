#include "ProductManager.h"
#include "Product.h"
#include "AstroShopping/SaveSystem/AstroShoppingSaveGame.h"
#include "AstroShopping/SaveSystem/PersistenceSubsystem.h"
#include "AstroShopping/SaveSystem/Structures/ProductDataSaveData.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AstroShopping/AstroShoppingPlayerController.h"
#include "AstroShopping/GameMode/GameGameMode.h"


AProductManager::AProductManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	NumberOfClientsFinishedLoadingProductDataMesh = 99;
	bHasStartedLoading = false;
}

void AProductManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProductManager, Products);
}

void AProductManager::BeginPlay()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World context is missing for product manager"));
		return;
	}

	AGameGameMode* GameMode = Cast<AGameGameMode>(GetWorld()->GetAuthGameMode());

	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameMode"));
		return;
	}

	GameMode->OnAllPlayersJoinedNative.AddLambda([this]()
		{
			GetGameInstance()->GetSubsystem<UPersistenceSubsystem>()->RequestLoad(this);
		});
}

void AProductManager::SaveData_Implementation(UAstroShoppingSaveGame* CurrentGameSave)
{
	if (!bHasStartedLoading)
	{
		UE_LOG(LogTemp, Error, TEXT("Product manager has not started loading"));
		return;
	}

	if (IsLoadingProductDataMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("Still loading save"));
		return;
	}

	CurrentGameSave->ProductData = ProductData;

	TArray<FProductSaveData> ProductSaveData;
	for (auto& Product : Products)
	{
		FProductSaveData SaveData;
		SaveData.Transform = Product->Mesh->GetComponentTransform();
		SaveData.ProductDataID = Product->ProductDataID;
		SaveData.Quantity = Product->Quantity;
		ProductSaveData.Add(SaveData);
	}
	CurrentGameSave->Products = ProductSaveData;
}

void AProductManager::LoadData_Implementation(UAstroShoppingSaveGame* CurrentGameSave)
{
	bHasStartedLoading = true;
	ProductDataToLoad = CurrentGameSave->ProductData;
	ProductsToLoad = CurrentGameSave->Products;
	LoadNextProductData();
}

void AProductManager::RequestNewProductSpawn(
	const FProductDataSaveData& Data,
	const FGuid& ProductDataID,
	const FTransform& Transform,
	const int32 Quantity
)
{
	ProductDataToLoad.Add(ProductDataID, Data);
	ProductsToLoad.Add(FProductSaveData{ Transform, ProductDataID, Quantity });

	if (ProductDataToLoad.Num() == 1)
	{
		LoadNextProductData();
	}
}

void AProductManager::LoadNextProductData()
{
	if (ProductDataToLoad.Num() == 0)
	{
		LoadProducts();
		return;
	}
	
	AddProductData(ProductDataToLoad.begin().Value(), ProductDataToLoad.begin().Key());
	ProductDataToLoad.Remove(ProductDataToLoad.begin().Key());
}

void AProductManager::LoadProducts()
{
	for (auto& ProductToLoad : ProductsToLoad)
	{
		SpawnProduct(ProductToLoad.Transform, ProductToLoad.ProductDataID, ProductToLoad.Quantity);
	}

	ProductsToLoad.Empty();
}

bool AProductManager::IsLoadingProductDataMesh() const
{
	if (CurrentlyLoadingProductDataID.IsValid())
	{
		return true;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World context is missing for product data mesh load completion"));
		return true;
	}

	AGameGameMode* GameMode = Cast<AGameGameMode>(GetWorld()->GetAuthGameMode());

	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameMode"));
		return true;
	}

	if (NumberOfClientsFinishedLoadingProductDataMesh < GameMode->GetNumPlayers())
	{
		return true;
	}

	return false;
}

bool AProductManager::AddProductData(const FProductDataSaveData& Data, const FGuid& ProductDataID)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("This function can only be called on the server"));
		return false;
	}

	if (IsLoadingProductDataMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("Product data mesh is still loading"));
		return false;
	}

	NumberOfClientsFinishedLoadingProductDataMesh = 0;

	ProductData.Add(ProductDataID, Data);

	Multicast_DownloadProductDataMesh(Data.MeshURL, ProductDataID);

	return true;
}

void AProductManager::Multicast_DownloadProductDataMesh_Implementation(const FString& MeshURL, FGuid ProductDataID)
{
	UE_LOG(LogTemp, Display, TEXT("Downloading product data mesh for url: %s"), *MeshURL);

	CurrentlyLoadingProductDataID = ProductDataID;

	TMap<FString, FString> Headers;
	FglTFRuntimeConfig LoaderConfig;
	FglTFRuntimeHttpResponse Delegate;
	Delegate.BindDynamic(this, &AProductManager::OnProductDataMeshDownloaded);

	UglTFRuntimeFunctionLibrary::glTFLoadAssetFromUrl(
		MeshURL,
		Headers,
		Delegate,
		LoaderConfig
	);
}

void AProductManager::OnProductDataMeshDownloaded(UglTFRuntimeAsset* Asset)
{
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;
	StaticMeshConfig.bBuildSimpleCollision = true;

	FglTFRuntimeStaticMeshAsync AsyncCallback;
	AsyncCallback.BindDynamic(this, &AProductManager::OnProductDataMeshLoaded);

	Asset->LoadStaticMeshAsync(0, AsyncCallback, StaticMeshConfig);
}

void AProductManager::OnProductDataMeshLoaded(UStaticMesh* Mesh)
{
	UE_LOG(LogTemp, Display, TEXT("Product data mesh for ID: %s loaded"), *CurrentlyLoadingProductDataID.ToString());

	ProductDataMeshes.Add(CurrentlyLoadingProductDataID, Mesh);

	CurrentlyLoadingProductDataID.Invalidate();

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World context is missing for product mesh setting"));
		return;
	}

	AAstroShoppingPlayerController* PlayerController = Cast<AAstroShoppingPlayerController>(GetWorld()->GetFirstPlayerController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player controller not found"));
		return;
	}

	PlayerController->Server_NotifyOfProductDataMeshLoadCompletion();
}

void AProductManager::Server_NotifyOfProductDataMeshLoadCompletion_Implementation()
{
	NumberOfClientsFinishedLoadingProductDataMesh++;

	if (IsLoadingProductDataMesh())
	{
		return;
	}

	LoadNextProductData();
}

class AProduct* AProductManager::SpawnProduct(
	const FTransform& Transform,
	const FGuid ProductDataID,
	const int32 Quantity
)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("This function can only be called on the server"));
		return nullptr;
	}

	if (!ProductData.Contains(ProductDataID))
	{
		UE_LOG(LogTemp, Error, TEXT("Product data not found"));
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!GetWorld())
	{
		return nullptr;
	}

	if (!ProductClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Product class not set"));
		return nullptr;
	}

	FProductDataSaveData ProductDataSaveData = ProductData[ProductDataID];

	AProduct* Product = GetWorld()->SpawnActorDeferred<AProduct>(
		ProductClass,
		Transform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	Product->ProductDataID = ProductDataID;
	Product->Name = ProductDataSaveData.Name;
	Product->Price = ProductDataSaveData.Price;
	Product->Quantity = Quantity;
	Product->FinishSpawning(Transform);

	Products.Add(Product);
	OnRep_Products();

	return Product;
}

void AProductManager::OnRep_Products()
{
	for (auto& Product : Products)
	{
		if (!Product)
		{
			continue;
		}

		if (Product->ProductMesh)
		{
			continue;
		}

		if (!ProductDataMeshes.Contains(Product->ProductDataID))
		{
			UE_LOG(LogTemp, Error, TEXT("Product data mesh with ID: %s not found"), *Product->ProductDataID.ToString());
			continue;
		}

		if (ProductDataMeshes[Product->ProductDataID] == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Product data mesh with ID: %s is null"), *Product->ProductDataID.ToString());
			continue;
		}

		Product->ProductMesh = ProductDataMeshes[Product->ProductDataID];

		UE_LOG(LogTemp, Display, TEXT("Product mesh set for ID: %s and product: %s from local role: %d"), *Product->ProductDataID.ToString(), *Product->GetFullName(), GetLocalRole());
	}
}

void AProductManager::DespawnProduct(AProduct* Product)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("This function can only be called on the server"));
		return;
	}

	if (!Product)
	{
		UE_LOG(LogTemp, Error, TEXT("Product is null"));
		return;
	}


	USceneComponent* RootComp = Product->GetRootComponent();
	if (RootComp)
	{
		RootComp->SetVisibility(false);
	}
	TArray<USceneComponent*> Components;
	Product->GetComponents<USceneComponent>(Components);
	for (USceneComponent* Comp : Components)
	{
		Comp->SetVisibility(false);
	}
	Product->SetActorEnableCollision(false);
	Product->Mesh->SetSimulatePhysics(false);
	Product->SetActorLocation(FVector(0, 0, -10000));

	Products.Remove(Product);
	OnRep_Products();
}
