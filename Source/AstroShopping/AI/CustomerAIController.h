#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CustomerAIController.generated.h"

UCLASS()
class ASTROSHOPPING_API ACustomerAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACustomerAIController();

	void TakeProduct(class AProduct* Product);

	AProduct* PutProduct();

	bool HasProducts() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinShelfsLeft = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxShelfsLeft = 12;

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UBlackboardComponent> BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName ShelfsLeftKeyName = TEXT("ShelfsLeft");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName ExitKeyName = TEXT("Exit");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<class AProduct*> Products;
};
