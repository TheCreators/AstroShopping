#include "BTT_FindCashRegister.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AstroShopping/FurnitureSystem/CashRegister.h"
#include "AIController.h"


UBTT_FindCashRegister::UBTT_FindCashRegister()
{
	NodeName = TEXT("Find Cash Register");
	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FindCashRegister, QueueIndexKey));
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FindCashRegister, CashRegisterActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTT_FindCashRegister::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid!"));
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> CashRegisters;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UCashRegister::StaticClass(), CashRegisters);

	CashRegisters.RemoveAll([](AActor* Register)
		{
			return ICashRegister::Execute_IsQueueFull(Register);
		});

	if (CashRegisters.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No cash registers found!"));
		return EBTNodeResult::Failed;
	}

	AActor* SelectedCashRegister = CashRegisters[0];
	int32 MinQueueSize = ICashRegister::Execute_GetQueueCurrentSize(SelectedCashRegister);

	for (AActor* Register : CashRegisters)
	{
		int32 CurrentQueueSize = ICashRegister::Execute_GetQueueCurrentSize(Register);
		if (CurrentQueueSize < MinQueueSize)
		{
			MinQueueSize = CurrentQueueSize;
			SelectedCashRegister = Register;
		}
	}

	ICashRegister::Execute_OccupyQueueSpot(SelectedCashRegister, MinQueueSize);

	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not valid!"));
		return EBTNodeResult::Failed;
	}

	AIController->GetBlackboardComponent()->SetValueAsObject(CashRegisterActorKey.SelectedKeyName, SelectedCashRegister);
	AIController->GetBlackboardComponent()->SetValueAsInt(QueueIndexKey.SelectedKeyName, MinQueueSize);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTT_FindCashRegister::GetStaticDescription() const
{
	return FString::Printf(TEXT("Cash Register Actor: %s\nQueue Index: %s"), *CashRegisterActorKey.SelectedKeyName.ToString(), *QueueIndexKey.SelectedKeyName.ToString());
}
