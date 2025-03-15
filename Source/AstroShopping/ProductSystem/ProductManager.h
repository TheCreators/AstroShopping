#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstroShopping/SaveSystem/Saveable.h"
#include "ProductManager.generated.h"

UCLASS()
class ASTROSHOPPING_API AProductManager : public AActor, public ISaveable
{
	GENERATED_BODY()
	
public:	
	AProductManager();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	class AProduct* SpawnProduct(const FTransform& Transform,
		const FGuid ID, const FString& Name,
		const TSoftObjectPtr<UStaticMesh> ProductMesh,
		const TSoftObjectPtr<UTexture2D> ProductThumbnail
	);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AProduct> ProductClass;

	void SaveData_Implementation(class UAstroShoppingSaveGame* CurrentGameSave) override;

	void LoadData_Implementation(class UAstroShoppingSaveGame* CurrentGameSave) override;

private:
	UPROPERTY()
	TMap<FGuid, TWeakObjectPtr<class AProduct>> Products;
};
