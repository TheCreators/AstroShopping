#include "BTT_TryAdvanceInQueue.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AstroShopping/FurnitureSystem/CashRegister.h"

UBTT_TryAdvanceInQueue::UBTT_TryAdvanceInQueue()
{
	NodeName = TEXT("Advance In Queue");
	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_TryAdvanceInQueue, QueueIndexKey));
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_TryAdvanceInQueue, CashRegisterActorKey), AActor::StaticClass());
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_TryAdvanceInQueue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not valid!"));
	}

	CashRegisterActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(CashRegisterActorKey.SelectedKeyName));
	QueueIndex = AIController->GetBlackboardComponent()->GetValueAsInt(QueueIndexKey.SelectedKeyName);

	return EBTNodeResult::InProgress;
}

void UBTT_TryAdvanceInQueue::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (ICashRegister::Execute_IsQueueSpotOccupied(CashRegisterActor, QueueIndex - 1))
	{
		return;
	}

	ICashRegister::Execute_OccupyQueueSpot(CashRegisterActor, QueueIndex - 1);
	ICashRegister::Execute_FreeQueueSpot(CashRegisterActor, QueueIndex);
	
	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsInt(QueueIndexKey.SelectedKeyName, QueueIndex - 1);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

FString UBTT_TryAdvanceInQueue::GetStaticDescription() const
{
	return FString::Printf(TEXT("Cash Register Actor: %s\nQueue Index: %s"), *CashRegisterActorKey.SelectedKeyName.ToString(), *QueueIndexKey.SelectedKeyName.ToString());
}
