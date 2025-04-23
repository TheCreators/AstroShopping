#pragma once

#include "CoreMinimal.h"
#include "StoreStatsSaveData.generated.h"

USTRUCT(BlueprintType)
struct FStoreStatsSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Balance = 200;

	UPROPERTY(BlueprintReadWrite)
	int32 PruductsSoldAmount = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 CustomersServedAmount = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 MoneyEarned = 0;

	UPROPERTY(BlueprintReadWrite)
	float AdsConversionValue = 1;
};
