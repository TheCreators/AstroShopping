#include "InteractionHintWidget.h"
#include "Components/TextBlock.h"

void UInteractionHintWidget::SetDisplayText(FString Text)
{
	if (!DisplayText) return;
	DisplayText->SetText(FText::FromString(Text));
}

void UInteractionHintWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
