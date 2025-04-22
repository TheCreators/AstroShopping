#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Despawn.generated.h"

UCLASS()
class ASTROSHOPPING_API UBTT_Despawn : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_Despawn();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
