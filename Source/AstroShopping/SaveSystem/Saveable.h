#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Saveable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class USaveable : public UInterface
{
	GENERATED_BODY()
};

class ASTROSHOPPING_API ISaveable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Saveable")
	void SaveData(class UAstroShoppingSaveGame* CurrentGameSave);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Saveable")
	void LoadData(class UAstroShoppingSaveGame* CurrentGameSave);
};
