#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameGameMode.generated.h"

UCLASS()
class ASTROSHOPPING_API AGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable)
	class AProductManager* GetProductManager() const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProductManager> ProductManagerClass;

private:
	UFUNCTION()
	void SpawnProductManager();

	TObjectPtr<class AProductManager> ProductManager;
};
