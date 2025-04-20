#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_HasNotFullCashRegisterQueue.generated.h"


UCLASS()
class ASTROSHOPPING_API UBTD_HasNotFullCashRegisterQueue : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
