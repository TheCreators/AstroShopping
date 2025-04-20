#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_TryAdvanceInQueue.generated.h"

UCLASS()
class ASTROSHOPPING_API UBTT_TryAdvanceInQueue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_TryAdvanceInQueue();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CashRegisterActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector QueueIndexKey;

	AActor* CashRegisterActor;

	int32 QueueIndex;
};
