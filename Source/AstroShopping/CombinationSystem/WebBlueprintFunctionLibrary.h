#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WebBlueprintFunctionLibrary.generated.h"

UCLASS()
class ASTROSHOPPING_API UWebBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	static void ClearWebBrowserCache();
};
