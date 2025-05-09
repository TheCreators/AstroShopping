#pragma once

#include "CoreMinimal.h"
#include "FurnitureLocationSaveData.generated.h"

USTRUCT(BlueprintType)
struct FFurnitureLocationSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FTransform> FurnitureTransforms;
};
