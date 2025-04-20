#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_LeaveCashRegister.generated.h"

UCLASS()
class ASTROSHOPPING_API UBTT_LeaveCashRegister : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_LeaveCashRegister();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CashRegisterActorKey;

	AActor* CashRegisterActor;
};
