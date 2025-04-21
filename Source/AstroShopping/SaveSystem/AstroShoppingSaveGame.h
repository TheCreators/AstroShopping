#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structures/ProductSaveData.h"
#include "Structures/StoreStatsSaveData.h"
#include "Structures/ProductDataSaveData.h"
#include "AstroShoppingSaveGame.generated.h"

UCLASS()
class ASTROSHOPPING_API UAstroShoppingSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	void SaveAllData(TArray<UObject*> SaveableObjects);

	void LoadDataForRequester(UObject* Requester);

	UPROPERTY()
	TArray<FProductSaveData> Products;

	UPROPERTY(BlueprintReadWrite)
	FStoreStatsSaveData StoreStats;

	UPROPERTY()
	TMap<FGuid, FProductDataSaveData> ProductData;
};
