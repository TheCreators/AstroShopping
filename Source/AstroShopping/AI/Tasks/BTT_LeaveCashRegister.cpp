#include "BTT_LeaveCashRegister.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AstroShopping/FurnitureSystem/CashRegister.h"

UBTT_LeaveCashRegister::UBTT_LeaveCashRegister()
{
	NodeName = TEXT("Leave Cash Register");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_LeaveCashRegister, CashRegisterActorKey), AActor::StaticClass());
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_LeaveCashRegister::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not valid!"));
	}

	CashRegisterActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(CashRegisterActorKey.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBTT_LeaveCashRegister::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!ICashRegister::Execute_IsPaymentComplete(CashRegisterActor))
	{
		return;
	}

	ICashRegister::Execute_FreeQueueSpot(CashRegisterActor, 0);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

FString UBTT_LeaveCashRegister::GetStaticDescription() const
{
	return FString::Printf(TEXT("Cash Register Actor: %s"), *CashRegisterActorKey.SelectedKeyName.ToString());
}
