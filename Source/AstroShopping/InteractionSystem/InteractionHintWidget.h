#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionHintWidget.generated.h"

UCLASS()
class ASTROSHOPPING_API UInteractionHintWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetDisplayText(FString Text);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

protected:
	virtual void NativeDestruct() override;
};
