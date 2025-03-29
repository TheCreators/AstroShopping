#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structures/ProductSaveData.h"
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
	TMap<FGuid, FProductSaveData> Products;

	UPROPERTY()
	TMap<FGuid, FProductDataSaveData> ProductData;
};
