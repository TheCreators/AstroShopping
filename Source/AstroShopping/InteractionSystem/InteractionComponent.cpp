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
	SetIsReplicatedByDefault(true);

	Owner = GetOwner();
}


void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

	Camera = Owner->FindComponentByClass<UCameraComponent>();
	bUseCamera = Camera != nullptr;

	HintWidgetComponent = NewObject<UWidgetComponent>(Owner);
	if (HintWidgetComponent)
	{
		HintWidgetComponent->RegisterComponent();
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			HintWidgetComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
		}
		Owner->AddInstanceComponent(HintWidgetComponent);
		HintWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HintWidgetComponent->SetDrawAtDesiredSize(true);

		if (HintWidgetClass)
		{
			HintWidget = CreateWidget<UInteractionHintWidget>(GetWorld(), HintWidgetClass);
			if (HintWidget)
			{
				HintWidgetComponent->SetWidget(HintWidget);
				HintWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Missing HintWidgetClass in %s"), *GetName());
			return;
		}
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceAccumulator += DeltaTime;
	if (TraceAccumulator < TraceInterval) return;
	TraceAccumulator = 0;

	FTransform TraceStart = bUseCamera && Camera.IsValid() ? Camera->GetComponentTransform() : Owner->GetActorTransform();

	FVector Start = TraceStart.GetLocation();
	FVector End = Start + TraceStart.GetRotation().GetForwardVector() * InteractionRange;

	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		InteractionTraceSphereRadius,
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
	if (Interactable.IsValid())
	{
		ServerInteract(Interactable.Get());
	}
}

void UInteractionComponent::RemoveInteractable()
{
	if (Interactable.IsValid())
	{
		Interactable.Reset();
		if (HintWidget)
		{
			HintWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInteractionComponent::UpdateInteractable(AActor* NewInteractable)
{
	if (HintWidgetComponent && NewInteractable)
	{
		if (const FVector WidgetLocation = IInteractable::Execute_GetInteractionHintLocation(NewInteractable); 
			!WidgetLocation.IsZero())
		{
			HintWidgetComponent->SetWorldLocation(WidgetLocation);
		}
	}

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

