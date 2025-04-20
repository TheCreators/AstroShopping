#include "BTD_HasNotFullCashRegisterQueue.h"
#include "Kismet/GameplayStatics.h"
#include "AstroShopping/FurnitureSystem/CashRegister.h"

bool UBTD_HasNotFullCashRegisterQueue::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid!"));
		return false;
	}

	TArray<AActor*> CashRegisters;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UCashRegister::StaticClass(), CashRegisters);

	for (AActor* Register : CashRegisters)
	{
		if (!ICashRegister::Execute_IsQueueFull(Register))
		{
			return true;
		}
	}

	return false;
}
