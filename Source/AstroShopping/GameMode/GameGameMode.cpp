#include "GameGameMode.h"
#include "AstroShopping/ProductSystem/ProductManager.h"

void AGameGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	for (const FString& Option : World->URL.Op)
	{
		if (Option.StartsWith("players="))
		{
			ExpectedNumberOfPlayers = FCString::Atoi(*Option.RightChop(8));
			break;
		}
	}

	SpawnProductManager();
}

void AGameGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 CurrentPlayers = GetNumPlayers();
    UE_LOG(LogTemp, Display, TEXT("Player joined. Current: %d, Expected: %d"), CurrentPlayers, ExpectedNumberOfPlayers);

    if (CurrentPlayers == ExpectedNumberOfPlayers)
    {
        FTimerHandle Handle;
        GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this]()
            {
                UE_LOG(LogTemp, Display, TEXT("All players joined"));
                OnAllPlayersJoined.Broadcast();
                OnAllPlayersJoinedNative.Broadcast();
            }), 1.0f, false);
    }
}

void AGameGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ExpectedNumberOfPlayers = GetNumPlayers();
}

AProductManager* AGameGameMode::GetProductManager() const
{
	return ProductManager;
}

void AGameGameMode::SpawnProductManager()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ProductManager = World->SpawnActor<AProductManager>(ProductManagerClass);
}
