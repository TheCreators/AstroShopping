#include "BTT_Despawn.h"
#include "AIController.h"

UBTT_Despawn::UBTT_Despawn()
{
	NodeName = TEXT("Despawn");
}

EBTNodeResult::Type UBTT_Despawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        APawn* AIPawn = AIController->GetPawn();
        if (AIPawn)
        {
            AIPawn->Destroy();
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}
