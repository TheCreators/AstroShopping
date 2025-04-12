#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CustomerBTTask_FindShelf.generated.h"

UCLASS(Blueprintable)
class ASTROSHOPPING_API UCustomerBTTask_FindShelf : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCustomerBTTask_FindShelf();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShelfLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShelfActorKey;
};
