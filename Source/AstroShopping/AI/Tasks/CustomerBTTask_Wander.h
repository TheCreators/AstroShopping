#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CustomerBTTask_Wander.generated.h"


UCLASS(Blueprintable)
class ASTROSHOPPING_API UCustomerBTTask_Wander : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCustomerBTTask_Wander();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WanderRadius = 1000.0f;
};
