#pragma once

#include "CoreMinimal.h"
#include "ProductDataSaveData.generated.h"

USTRUCT(BlueprintType)
struct FProductDataSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	int32 Price = 0;

	UPROPERTY(BlueprintReadWrite)
	FString MeshURL;
};
