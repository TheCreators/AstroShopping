#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "AstroShoppingGameInstance.generated.h"

UCLASS()
class ASTROSHOPPING_API UAstroShoppingGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	virtual void Shutdown() override;
};
