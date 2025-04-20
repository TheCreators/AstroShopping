#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_PlaceProduct.generated.h"


UCLASS()
class ASTROSHOPPING_API UBTT_PlaceProduct : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_PlaceProduct();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CashRegisterActorKey;

	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PutAllProductsKey;

	AActor* CashRegisterActor;

	class AProduct* Product;
};
