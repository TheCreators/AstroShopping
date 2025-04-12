#include "CustomerBTTask_TakeProducts.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UCustomerBTTask_TakeProducts::UCustomerBTTask_TakeProducts()
{
	NodeName = TEXT("Take Products");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UCustomerBTTask_TakeProducts, ShelfActorKey), AActor::StaticClass());
	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UCustomerBTTask_TakeProducts, ShelfsLeftKey));
}

EBTNodeResult::Type UCustomerBTTask_TakeProducts::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

	int32 ShelfsLeft = BlackboardComp->GetValueAsInt(ShelfsLeftKey.SelectedKeyName);
	BlackboardComp->SetValueAsInt(ShelfsLeftKey.SelectedKeyName, ShelfsLeft - 1);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UCustomerBTTask_TakeProducts::GetStaticDescription() const
{
	return FString::Printf(TEXT("Shelf: %s, Shelfs Left: %s"), *ShelfActorKey.SelectedKeyName.ToString(), *ShelfsLeftKey.SelectedKeyName.ToString());
}
