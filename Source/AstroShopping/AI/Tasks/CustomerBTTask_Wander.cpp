#include "CustomerBTTask_Wander.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UCustomerBTTask_Wander::UCustomerBTTask_Wander()
{
	NodeName = TEXT("Wander");
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UCustomerBTTask_Wander, BlackboardKey));
}

EBTNodeResult::Type UCustomerBTTask_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation Location{};

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController->GetPawn();

	FVector Origin = Pawn->GetActorLocation();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(Origin, WanderRadius, Location))
	{
		AIController->GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Location.Location);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UCustomerBTTask_Wander::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *GetSelectedBlackboardKey().ToString());
}
