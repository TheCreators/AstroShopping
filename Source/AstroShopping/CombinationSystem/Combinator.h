#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstroShopping/AI/LlmApiClient.h"
#include "AstroShopping/AI/GenieApiClient.h"
#include "Combinator.generated.h"

DECLARE_DELEGATE(FGotSelectedProductModelUrl)

UCLASS()
class ASTROSHOPPING_API ACombinator : public AActor
{
	GENERATED_BODY()
	
public:
	ACombinator();

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Combination")
	void Server_StartCombination(
		const FString& FirstProductName,
		const FString& SecondProductName,
		const int32 FirstProductPrice,
		const int32 SecondProductPrice
	);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_Progress();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_ProductName();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_SelectedProductIndex();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_ProductThumbnailUrls();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowError(const FString& ErrorMessage);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Combination")
	void Server_RequestLoadProduct(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	FTransform GetProductSpawnPoint() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnReset();

	UFUNCTION(BlueprintPure, Category = "Combination")
	bool IsWorking() const;

	UPROPERTY(BlueprintReadOnly, Category = "Progress")
	int32 MaxProgress;

	UPROPERTY(ReplicatedUsing = OnRep_Progress, BlueprintReadOnly, Category = "Progress")
	int32 Progress;

	UPROPERTY(ReplicatedUsing = OnRep_ProductName, BlueprintReadOnly, Category = "Product")
	FString ProductName;

	UPROPERTY(BlueprintReadOnly, Category = "Product")
	int32 ProductPrice;

	UPROPERTY(ReplicatedUsing = OnRep_ProductThumbnailUrls, BlueprintReadOnly, Category = "Product")
	TArray<FString> ProductThumbnailUrls;

	UPROPERTY(ReplicatedUsing = OnRep_SelectedProductIndex, BlueprintReadOnly, Category = "Product")
	int32 SelectedProductIndex;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Reset();

	void GenerateCombinedItemProps(const FString& InputText, TFunction<void(FString, FString, int32)> OnSuccess, TFunction<void(FString)> OnError);

	void GenerateCombinedItemModels(const FString& Prompt, TFunction<void(FString)> OnError);

	void PollModelStatus(const FString& ModelId, int32 AttemptsRemaining, int32 ModelIndex, TFunction<void(FString)> OnError);

	void IncreaseProgress();

	FGotSelectedProductModelUrl GotSelectedProductModelUrl;

	UPROPERTY()
	TArray<FString> ProductModelUrls;

	TArray<bool> ProductThumbnailLoaded;

	TUniquePtr<FLlmApiClient> LlmApiClient;

	TUniquePtr<FGenieApiClient> GenieApiClient;

	int32 MaxPollingAttempts;

	float PollDelaySeconds;
};
