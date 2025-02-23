#include "InteractionComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "InteractionHintWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interactable.h"
#include "Net/UnrealNetwork.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	Owner = GetOwner();
}


void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

	if (!GetOwner()->HasLocalNetOwner())
	{
		return;
	}

	Camera = Owner->FindComponentByClass<UCameraComponent>();
	bUseCamera = Camera != nullptr;

	HintWidgetComponent = NewObject<UWidgetComponent>(Owner);
	if (HintWidgetComponent)
	{
		HintWidgetComponent->RegisterComponent();
		HintWidgetComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		Owner->AddInstanceComponent(HintWidgetComponent);
		HintWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HintWidgetComponent->SetDrawAtDesiredSize(true);

		HintWidget = CreateWidget<UInteractionHintWidget>(GetWorld(), HintWidgetClass);
		if (HintWidget)
		{
			HintWidgetComponent->SetWidget(HintWidget);
			HintWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FTransform TraceStart = bUseCamera ? Camera->GetComponentTransform() : Owner->GetActorTransform();

	FVector Start = TraceStart.GetLocation();
	FVector End = Start + TraceStart.GetRotation().GetForwardVector() * InteractionRange;

	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		InteractionTraceSchereRadius,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2),
		false,
		TArray<AActor*>{Owner},
		bDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green
	);

	if (!bHit)
	{
		RemoveInteractable();
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	if (!HitActor->Implements<UInteractable>() || !IInteractable::Execute_CanInteract(HitActor))
	{
		RemoveInteractable();
		return;
	}

	UpdateInteractable(HitActor);
}

void UInteractionComponent::ServerInteract_Implementation(AActor* TargetInteractable)
{
	if (TargetInteractable)
	{
		IInteractable::Execute_Interact(TargetInteractable, Owner);
	}
}

void UInteractionComponent::TryInteract()
{
	if (Interactable)
	{
		ServerInteract(Interactable);
	}
}

void UInteractionComponent::RemoveInteractable()
{
	if (Interactable)
	{
		Interactable = nullptr;
		if (HintWidget)
		{
			HintWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInteractionComponent::UpdateInteractable(AActor* NewInteractable)
{
	if (HintWidgetComponent)
	{
		HintWidgetComponent->SetWorldLocation(IInteractable::Execute_GetInteractionHintLocation(NewInteractable));
	}
	
	if (NewInteractable == Interactable)
	{
		return;
	}

	Interactable = NewInteractable;
	if (HintWidget)
	{
		HintWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

