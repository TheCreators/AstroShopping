#include "CustomerBTTask_TakeProducts.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AstroShopping/FurnitureSystem/ProductsContainer.h"
#include "AstroShopping/ProductSystem/Product.h"
#include "AstroShopping/AI/CustomerAIController.h"

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

	if (!IsValid(BlackboardComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("Blackboard component is not valid!"));
		return EBTNodeResult::Failed;
	}

	AActor* ShelfActor = Cast<AActor>(BlackboardComp->GetValueAsObject(ShelfActorKey.SelectedKeyName));

	if (!IsValid(ShelfActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Shelf actor is not valid!"));
		return EBTNodeResult::Failed;
	}

	TArray<AProduct*> AvailableProducts = IProductsContainer::Execute_GetProducts(ShelfActor);

	AProduct* ProductToTake = AvailableProducts.Num() > 0 ? AvailableProducts[0] : nullptr;

	if (!IsValid(ProductToTake))
	{
		UE_LOG(LogTemp, Warning, TEXT("No products available to take!"));
		return EBTNodeResult::Failed;
	}

	AProduct* TakenProduct = IProductsContainer::Execute_TakeProduct(ShelfActor, ProductToTake);

	if (!IsValid(TakenProduct))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to take product!"));
		return EBTNodeResult::Failed;
	}

	ACustomerAIController* CustomerAIController = Cast<ACustomerAIController>(AIController);

	if (!IsValid(CustomerAIController))
	{
		UE_LOG(LogTemp, Warning, TEXT("Customer AI controller is not valid!"));
		return EBTNodeResult::Failed;
	}

	CustomerAIController->TakeProduct(TakenProduct);

	int32 ShelfsLeft = BlackboardComp->GetValueAsInt(ShelfsLeftKey.SelectedKeyName);
	BlackboardComp->SetValueAsInt(ShelfsLeftKey.SelectedKeyName, ShelfsLeft - 1);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UCustomerBTTask_TakeProducts::GetStaticDescription() const
{
	return FString::Printf(TEXT("Shelf: %s\nShelfs Left: %s"), *ShelfActorKey.SelectedKeyName.ToString(), *ShelfsLeftKey.SelectedKeyName.ToString());
}
