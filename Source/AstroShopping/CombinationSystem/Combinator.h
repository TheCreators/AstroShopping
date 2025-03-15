#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstroShopping/AI/LlmApiClient.h"
#include "AstroShopping/AI/GenieApiClient.h"
#include "Combinator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProductThumbnailLoaded, int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReseted);

UCLASS()
class ASTROSHOPPING_API ACombinator : public AActor
{
	GENERATED_BODY()
	
public:
	ACombinator();

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Combination")
	void Server_StartCombination(const FString& Product1Name, const FString& Product2Name);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_Progress();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_ProductName();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Combination")
	void Server_RequestLoadProduct(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	FTransform GetProductSpawnPoint() const;

	UPROPERTY(BlueprintAssignable, Category = "Combination")
	FReseted OnCombinatorReset;

	UPROPERTY(BlueprintAssignable, Category = "Combination")
	FProductThumbnailLoaded OnProductThumbnailLoaded;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combination")
	bool bIsCombining;

	UPROPERTY(BlueprintReadOnly, Category = "Progress")
	int32 MaxProgress;

	UPROPERTY(ReplicatedUsing = OnRep_Progress, BlueprintReadOnly, Category = "Progress")
	int32 Progress;

	UPROPERTY(ReplicatedUsing = OnRep_ProductName, BlueprintReadOnly, Category = "Product")
	FString ProductName;

	UPROPERTY(BlueprintReadOnly, Category = "Product")
	TArray<UTexture2D*> ProductThumbnails;

	UPROPERTY(BlueprintReadOnly, Category = "Product")
	bool bIsProductSelected;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Reset();

	UFUNCTION()
	void OnModelDownloaded(UglTFRuntimeAsset* Asset);

	UFUNCTION()
	void OnModelLoaded(UStaticMesh* Mesh);

	void GenerateCombinedItemProps(const FString& InputText, TFunction<void(FString, FString)> OnSuccess, TFunction<void(FString)> OnError);

	void GenerateCombinedItemModels(const FString& Prompt, TFunction<void(FString)> OnError);

	void PollModelStatus(const FString& ModelId, int32 AttemptsRemaining, int32 ModelIndex, TFunction<void(FString)> OnError);

	void IncreaseProgress();

	void ResetProgress();

	void TryLoadProduct();

	void TrySetProductMesh();

	UFUNCTION()
	void OnRep_ProductThumbnailUrls();

	UFUNCTION()
	void OnRep_Product();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LoadProduct(int32 Index);

	UPROPERTY(Replicated)
	TArray<FString> ProductModelUrls;

	UPROPERTY(ReplicatedUsing = OnRep_ProductThumbnailUrls)
	TArray<FString> ProductThumbnailUrls;

	UPROPERTY(ReplicatedUsing = OnRep_Product)
	TObjectPtr<class AProduct> Product;

	TObjectPtr<class AProduct> LocalProduct;

	TObjectPtr<UStaticMesh> ProductMesh;

	int32 SelectedProductIndex;

	TUniquePtr<FLlmApiClient> LlmApiClient;

	TUniquePtr<FGenieApiClient> GenieApiClient;

	int32 MaxPollingAttempts;

	float PollDelaySeconds;
};
