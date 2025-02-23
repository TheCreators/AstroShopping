#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTROSHOPPING_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

	void TryInteract();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* TargetInteractable);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float InteractionRange = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float InteractionTraceSphereRadius = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInteractionHintWidget> HintWidgetClass;

	void RemoveInteractable();

	void UpdateInteractable(AActor* NewInteractable);

	bool bUseCamera;

	AActor* Owner;

	class UCameraComponent* Camera;

	AActor* Interactable;

	class UWidgetComponent* HintWidgetComponent;

	class UUserWidget* HintWidget;
};
