#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstroShopping/SaveSystem/Saveable.h"
#include "AstroShopping/SaveSystem/Structures/ProductSaveData.h"
#include "AstroShopping/SaveSystem/Structures/ProductDataSaveData.h"
#include "ProductManager.generated.h"

UCLASS()
class ASTROSHOPPING_API AProductManager : public AActor, public ISaveable
{
	GENERATED_BODY()
	
public:	
	AProductManager();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool AddProductData(const FProductDataSaveData& Data, const FGuid& ProductDataID);

	UFUNCTION(BlueprintCallable)
	class AProduct* SpawnProduct(
		const FTransform& Transform,
		const FGuid ProductDataID,
		const int32 Quantity
	);

	UFUNCTION(BlueprintCallable)
	void RequestNewProductSpawn(
		const FProductDataSaveData& Data, 
		const FGuid& ProductDataID,
		const FTransform& Transform,
		const int32 Quantity
	);

	UFUNCTION(BlueprintCallable)
	void DespawnProduct(AProduct* Product);

	void SaveData_Implementation(class UAstroShoppingSaveGame* CurrentGameSave) override;

	void LoadData_Implementation(class UAstroShoppingSaveGame* CurrentGameSave) override;

	UFUNCTION(Server, Reliable)
	void Server_NotifyOfProductDataMeshLoadCompletion();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartedLoading();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishedLoading();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AProduct> ProductClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FGuid, FProductDataSaveData> ProductData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FGuid, UStaticMesh*> ProductDataMeshes;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DownloadProductDataMesh(
		const FString& MeshURL, 
		FGuid ProductDataID
	);

	UFUNCTION()
	void OnProductDataMeshDownloaded(UglTFRuntimeAsset* Asset);

	UFUNCTION()
	void OnProductDataMeshLoaded(UStaticMesh* Mesh);

	UFUNCTION()
	void OnRep_Products();

	UFUNCTION()
	void LoadNextProductData();

	void LoadProducts();

	TMap<FGuid, FProductDataSaveData> ProductDataToLoad;

	TArray<FProductSaveData> ProductsToLoad;

	bool IsLoadingProductDataMesh() const;

	UPROPERTY(ReplicatedUsing = OnRep_Products)
	TArray<TObjectPtr<class AProduct>> Products;

	int32 NumberOfClientsFinishedLoadingProductDataMesh;

	FGuid CurrentlyLoadingProductDataID;

	bool bHasStartedLoading;

	bool bHasFinishedLoading;
};
