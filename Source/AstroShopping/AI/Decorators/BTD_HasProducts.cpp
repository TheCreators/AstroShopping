
#include "BTD_HasProducts.h"
#include "AstroShopping/AI/CustomerAIController.h"

bool UBTD_HasProducts::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not valid!"));
	}

	ACustomerAIController* CustomerAIController = Cast<ACustomerAIController>(AIController);

	if (!CustomerAIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Customer AI controller is not valid!"));
		return false;
	}

	return CustomerAIController->HasProducts();
}
