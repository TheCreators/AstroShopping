#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString Text)
{
	if (!DisplayText) return;

	DisplayText->SetText(FText::FromString(Text));
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;

	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = "Authority";
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = "AutonomousProxy";
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = "SimulatedProxy";
		break;
	case ENetRole::ROLE_None:
		Role = "None";
		break;
	}
	
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}