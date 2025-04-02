#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class ASTROSHOPPING_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALobbyGameMode();

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintPure)
	bool HasLoadingPlayers() const;

	UPROPERTY(EditDefaultsOnly)
	FString GameLevelName;

private:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	int32 LoadingPlayers = 0;
};
