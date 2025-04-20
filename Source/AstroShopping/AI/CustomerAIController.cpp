#include "CustomerAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ACustomerAIController::ACustomerAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ACustomerAIController::BeginPlay()
{
	Super::BeginPlay();

	BlackboardComponent->SetValueAsInt(ShelfsLeftKeyName, 3);
	
	if (IsValid(BehaviorTree))
	{
		RunBehaviorTree(BehaviorTree);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
}

void ACustomerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (Blackboard && BehaviorTree)
	{
		Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}

void ACustomerAIController::TakeProduct(AProduct* Product)
{
	Products.Add(Product);
}

AProduct* ACustomerAIController::PutProduct()
{
	if (Products.Num() > 0)
	{
		AProduct* Product = Products[0];
		Products.RemoveAt(0);
		return Product;
	}
	return nullptr;
}

bool ACustomerAIController::HasProducts() const
{
	return Products.Num() > 0;
}
