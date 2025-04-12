#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CustomerBTTask_TakeProducts.generated.h"

UCLASS()
class ASTROSHOPPING_API UCustomerBTTask_TakeProducts : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCustomerBTTask_TakeProducts();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShelfActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShelfsLeftKey;
};
