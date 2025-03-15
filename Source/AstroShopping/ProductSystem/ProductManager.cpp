#include "ProductManager.h"
#include "Product.h"
#include "AstroShopping/SaveSystem/AstroShoppingSaveGame.h"
#include "AstroShopping/SaveSystem/PersistenceSubsystem.h"

AProductManager::AProductManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProductManager::BeginPlay()
{
	GetGameInstance()->GetSubsystem<UPersistenceSubsystem>()->RequestLoad(this);
}

class AProduct* AProductManager::SpawnProduct(
	const FTransform& Transform, 
	const FGuid ID, const FString& Name, 
	const TSoftObjectPtr<UStaticMesh> ProductMesh, 
	const TSoftObjectPtr<UTexture2D> ProductThumbnail
)
{
	if (!HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This function can only be called on the server"));
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!GetWorld())
	{
		return nullptr;
	}

	if (!ProductClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Product class not set"));
		return nullptr;
	}

	AProduct* Product = GetWorld()->SpawnActorDeferred<AProduct>(
		ProductClass, 
		Transform, 
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);
	
	Product->ID = ID;
	Product->Name = Name;
	Product->ProductMesh = ProductMesh;
	Product->ProductThumbnail = ProductThumbnail;
	Product->FinishSpawning(Transform);

	Products.Add(ID, Product);

	return Product;
}

void AProductManager::SaveData_Implementation(UAstroShoppingSaveGame* CurrentGameSave)
{
	TMap<FGuid, FProductSaveData> ProductSaveData;

	for (auto& Product : Products)
	{
		FProductSaveData SaveData;
		SaveData.Name = Product.Value->Name;
		SaveData.Transform = Product.Value->Mesh->GetComponentTransform();
		ProductSaveData.Add(Product.Key, SaveData);
	}

	CurrentGameSave->Products = ProductSaveData;
}

void AProductManager::LoadData_Implementation(UAstroShoppingSaveGame* CurrentGameSave)
{
	for (auto& Product : CurrentGameSave->Products)
	{
		SpawnProduct(Product.Value.Transform, Product.Key, Product.Value.Name, nullptr, nullptr);
	}
}
