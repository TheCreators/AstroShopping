#include "BTT_GetLocationInQueue.h"
#include "AstroShopping/FurnitureSystem/CashRegister.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_GetLocationInQueue::UBTT_GetLocationInQueue()
{
	NodeName = TEXT("Get Location In Queue");
	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_GetLocationInQueue, QueueIndexKey));
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_GetLocationInQueue, CashRegisterActorKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_GetLocationInQueue, LocationInQueueKey));
}

EBTNodeResult::Type UBTT_GetLocationInQueue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not valid!"));
		return EBTNodeResult::Failed;
	}

	AActor* CashRegisterActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(CashRegisterActorKey.SelectedKeyName));
	int32 QueueIndex = AIController->GetBlackboardComponent()->GetValueAsInt(QueueIndexKey.SelectedKeyName);

	FVector LocationInQueue = ICashRegister::Execute_GetLocationInQueue(CashRegisterActor, QueueIndex);

	AIController->GetBlackboardComponent()->SetValueAsVector(LocationInQueueKey.SelectedKeyName, LocationInQueue);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTT_GetLocationInQueue::GetStaticDescription() const
{
	return FString::Printf(TEXT("Cash Register Actor: %s\nQueue Index: %s\nLocation In Queue: %s"), *CashRegisterActorKey.SelectedKeyName.ToString(), *QueueIndexKey.SelectedKeyName.ToString(), *LocationInQueueKey.SelectedKeyName.ToString());
}
