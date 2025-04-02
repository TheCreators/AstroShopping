#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"


ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;

	GameLevelName = TEXT("SpaceStation");
}

void ALobbyGameMode::StartGame()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	int32 NumberOfPlayer = GameState.Get()->PlayerArray.Num();

	FString TravelURL = FString::Printf(TEXT("/Game/AstroShopping/Maps/%s?listen?players=%d"), *GameLevelName, NumberOfPlayer);
	World->ServerTravel(*TravelURL);
}

bool ALobbyGameMode::HasLoadingPlayers() const
{
	return LoadingPlayers > 0;
}

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	LoadingPlayers++;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	LoadingPlayers--;
}
