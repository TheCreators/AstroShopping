#include "BTD_HasNotEmptyProductContainers.h"
#include "Kismet/GameplayStatics.h"
#include "AstroShopping/FurnitureSystem/ProductsContainer.h"

bool UBTD_HasNotEmptyProductContainers::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid!"));
		return false;
	}

	TArray<AActor*> ProductsContainers;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UProductsContainer::StaticClass(), ProductsContainers);

	for (AActor* Container : ProductsContainers)
	{
		if (IProductsContainer::Execute_HasProducts(Container))
		{
			return true;
		}
	}

	return false;
}
