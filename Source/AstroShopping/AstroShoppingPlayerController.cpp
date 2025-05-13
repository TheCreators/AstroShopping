#include "AstroShoppingPlayerController.h"
#include "AstroShopping/GameMode/GameGameMode.h"
#include "AstroShopping/ProductSystem/ProductManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

void AAstroShoppingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle InputInitHandle;
	GetWorldTimerManager().SetTimer(InputInitHandle, [this]()
		{
			if (IsLocalController())
			{
				FInputModeGameOnly GameInputMode;
				SetInputMode(GameInputMode);
				bShowMouseCursor = false;
				SetupInputSystem();
			}
		}, 1.0f, false);
}

void AAstroShoppingPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		FInputModeGameOnly GameInputMode;
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
		SetupInputSystem();
	}
}

void AAstroShoppingPlayerController::SetupInputSystem()
{
	if (DefaultMappingContext && GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("Input context added for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to setup input system for %s"), *GetName());
	}

	FInputModeGameOnly GameInputMode;
	SetInputMode(GameInputMode);
	bShowMouseCursor = false;
}
