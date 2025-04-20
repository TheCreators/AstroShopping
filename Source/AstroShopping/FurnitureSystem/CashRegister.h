#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CashRegister.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UCashRegister : public UInterface
{
	GENERATED_BODY()
};

class ASTROSHOPPING_API ICashRegister
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Queue")
	bool IsQueueFull() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Queue")
	int32 GetQueueCurrentSize() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Queue")
	FVector GetLocationInQueue(int32 Index) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Queue")
	void OccupyQueueSpot(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Queue")
	void FreeQueueSpot(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Queue")
	bool IsQueueSpotOccupied(int32 Index) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Sale")
	bool CanAcceptProduct() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Sale")
	void PlaceProduct(class AProduct* Product);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Sale")
	void AllProductsPlaced();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Sale")
	bool IsPaymentComplete() const;
};
