#include "GameGameMode.h"
#include "AstroShopping/ProductSystem/ProductManager.h"

void AGameGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	SpawnProductManager();
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
