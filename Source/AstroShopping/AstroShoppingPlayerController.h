#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AstroShoppingPlayerController.generated.h"

UCLASS()
class ASTROSHOPPING_API AAstroShoppingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void Server_NotifyOfProductDataMeshLoadCompletion();

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UFUNCTION()
	void SetupInputSystem();
};
