#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstroShopping/AI/LlmApiClient.h"
#include "Combinator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombinationComplete, UStaticMesh*, ResultMesh);

UCLASS()
class ASTROSHOPPING_API ACombinator : public AActor
{
	GENERATED_BODY()
	
public:
	ACombinator();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Combination")
	void Server_StartCombination(const FString& Product1Name, const FString& Product2Name);

	UPROPERTY(BlueprintAssignable, Category = "Combination")
	FCombinationComplete OnCombinationComplete;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combination")
	bool bIsCombining = false;

protected:

private:
	UPROPERTY(ReplicatedUsing = OnRep_ModelUrls)
	TArray<FString> ModelUrls;

	UFUNCTION()
	void OnRep_ModelUrls();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnModelDownloaded(UglTFRuntimeAsset* Asset);

	UFUNCTION()
	void OnModelLoaded(UStaticMesh* StaticMesh);

	TUniquePtr<FLlmApiClient> LlmApiClient;

	void GenerateCombinedItemProps(const FString& InputText, TFunction<void(FString, FString)> OnSuccess, TFunction<void(FString)> OnError);
};
