#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AstroShoppingCharacter.generated.h"

UCLASS()
class ASTROSHOPPING_API AAstroShoppingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAstroShoppingCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FirstPersonCameraComponent;

protected:
	virtual void BeginPlay() override;

	void Move(const struct FInputActionValue& Value);

	void Look(const struct FInputActionValue& Value);

	void Interact(const struct FInputActionValue& Value);

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, Category = "Debug", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY()
	float MouseSensitivity;
};
