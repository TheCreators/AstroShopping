#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllPlayersJoined);
DECLARE_MULTICAST_DELEGATE(FAllPlayersJoinedNative);

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

	UPROPERTY(BlueprintAssignable)
	FAllPlayersJoined OnAllPlayersJoined;

	FAllPlayersJoinedNative OnAllPlayersJoinedNative;

private:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION()
	void SpawnProductManager();

	TObjectPtr<class AProductManager> ProductManager;

	int32 ExpectedNumberOfPlayers;
};
