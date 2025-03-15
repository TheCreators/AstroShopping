#pragma once

#include "CoreMinimal.h"
#include "ProductSaveData.generated.h"

USTRUCT(BlueprintType)
struct FProductSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;

	UPROPERTY(BlueprintReadWrite)
	FString Name;
};
