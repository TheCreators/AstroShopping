#include "AstroShoppingPlayerController.h"
#include "AstroShopping/GameMode/GameGameMode.h"
#include "AstroShopping/ProductSystem/ProductManager.h"

void AAstroShoppingPlayerController::Server_NotifyOfProductDataMeshLoadCompletion_Implementation()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World context is missing for product data mesh load completion"));
		return;
	}

	AGameGameMode* GameMode = Cast<AGameGameMode>(GetWorld()->GetAuthGameMode());

	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameMode"));
		return;
	}

	AProductManager* ProductManager = GameMode->GetProductManager();

	if (!ProductManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Product manager not found"));
		return;
	}

	ProductManager->Server_NotifyOfProductDataMeshLoadCompletion();
}
