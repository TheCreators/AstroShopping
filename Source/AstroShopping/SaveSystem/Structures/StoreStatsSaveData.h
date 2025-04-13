#pragma once

#include "CoreMinimal.h"
#include "StoreStatsSaveData.generated.h"

USTRUCT(BlueprintType)
struct FStoreStatsSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Balance;

	UPROPERTY(BlueprintReadWrite)
	int32 PruductsSoldAmount;

	UPROPERTY(BlueprintReadWrite)
	int32 CustomersServedAmount;

	UPROPERTY(BlueprintReadWrite)
	int32 MoneyEarned;

	UPROPERTY(BlueprintReadWrite)
	float AdsConversionValue = 1;
};
