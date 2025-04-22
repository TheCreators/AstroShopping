#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BotSpawner.generated.h"

UCLASS()
class ASTROSHOPPING_API ABotSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABotSpawner();

	UFUNCTION(BlueprintImplementableEvent)
	float GetAdsConversionRate() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> BotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	float BaseSpawnDelay = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	int32 BaseMaxBots = 3;

protected:
	virtual void BeginPlay() override;

private:
	int32 CurrentBotCount = 0;

	UFUNCTION()
	void ScheduleNextSpawn();

	UFUNCTION()
	void SpawnBot();

	UFUNCTION()
	void OnBotDestroyed(AActor* DestroyedActor);
};
