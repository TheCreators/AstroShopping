#include "BTT_PlaceProduct.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AstroShopping/FurnitureSystem/CashRegister.h"
#include "AstroShopping/AI/CustomerAIController.h"

UBTT_PlaceProduct::UBTT_PlaceProduct()
{
	NodeName = TEXT("Place Product");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_PlaceProduct, CashRegisterActorKey), AActor::StaticClass());
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_PlaceProduct, PutAllProductsKey));
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_PlaceProduct::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not valid!"));
	}

	CashRegisterActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(CashRegisterActorKey.SelectedKeyName));
	
	ACustomerAIController* CustomerAIController = Cast<ACustomerAIController>(AIController);

	if (!CustomerAIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Customer AI controller is not valid!"));
		return EBTNodeResult::Failed;
	}

	if (!CustomerAIController->HasProducts())
	{
		ICashRegister::Execute_AllProductsPlaced(CashRegisterActor);
		AIController->GetBlackboardComponent()->SetValueAsBool(PutAllProductsKey.SelectedKeyName, true);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	Product = CustomerAIController->PutProduct();

	return EBTNodeResult::InProgress;
}

void UBTT_PlaceProduct::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!ICashRegister::Execute_CanAcceptProduct(CashRegisterActor))
	{
		return;
	}

	ICashRegister::Execute_PlaceProduct(CashRegisterActor, Product);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

FString UBTT_PlaceProduct::GetStaticDescription() const
{
	return FString::Printf(TEXT("Cash Register Actor: %s"), *CashRegisterActorKey.SelectedKeyName.ToString());
}
