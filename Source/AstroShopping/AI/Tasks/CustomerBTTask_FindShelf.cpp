#include "CustomerBTTask_FindShelf.h"
#include <Kismet/GameplayStatics.h>
#include "AstroShopping/FurnitureSystem/ProductsContainer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UCustomerBTTask_FindShelf::UCustomerBTTask_FindShelf()
{
	NodeName = TEXT("Find Shelf");
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UCustomerBTTask_FindShelf, ShelfLocationKey));
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UCustomerBTTask_FindShelf, ShelfActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UCustomerBTTask_FindShelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TArray<AActor*> ProductsContainers;

	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid!"));
		return EBTNodeResult::Failed;
	}

	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UProductsContainer::StaticClass(), ProductsContainers);

	ProductsContainers.RemoveAll([](AActor* Container)
		{
			return !IProductsContainer::Execute_HasProducts(Container);
		});

	if (ProductsContainers.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No products containers found!"));
		return EBTNodeResult::Failed;
	}

	AActor* RandomProductContainer = ProductsContainers[FMath::RandRange(0, ProductsContainers.Num() - 1)];	
	FVector Location = IProductsContainer::Execute_GetInteractionLocation(RandomProductContainer);

	AAIController* AIController = OwnerComp.GetAIOwner();
	AIController->GetBlackboardComponent()->SetValueAsVector(ShelfLocationKey.SelectedKeyName, Location);
	AIController->GetBlackboardComponent()->SetValueAsObject(ShelfActorKey.SelectedKeyName, RandomProductContainer);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UCustomerBTTask_FindShelf::GetStaticDescription() const
{
	return FString::Printf(TEXT("Shelf Location: %s, Shelf Actor: %s"), *ShelfLocationKey.SelectedKeyName.ToString(), *ShelfActorKey.SelectedKeyName.ToString());
}
