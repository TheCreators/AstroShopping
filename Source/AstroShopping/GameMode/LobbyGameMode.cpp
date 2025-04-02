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

	World->ServerTravel(TEXT("/Game/AstroShopping/Maps/" + GameLevelName + "?listen?players=" + FString::FromInt(NumberOfPlayer)));
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
