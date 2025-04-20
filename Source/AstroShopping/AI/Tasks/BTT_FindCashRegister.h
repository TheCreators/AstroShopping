#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindCashRegister.generated.h"

UCLASS()
class ASTROSHOPPING_API UBTT_FindCashRegister : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_FindCashRegister();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CashRegisterActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector QueueIndexKey;
};
