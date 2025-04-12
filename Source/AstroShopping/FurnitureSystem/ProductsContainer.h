#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProductsContainer.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UProductsContainer : public UInterface
{
	GENERATED_BODY()
};

class ASTROSHOPPING_API IProductsContainer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProductContainer")
	FVector GetInteractionLocation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProductContainer")
	bool HasProducts() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProductContainer")
	TArray<TObjectPtr<AProduct>> GetProducts() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProductContainer")
	TObjectPtr<AProduct> TakeProduct(const FGuid& ProductID);
};
