#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PersistenceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSaveGameCompleteDelegate, UAstroShoppingSaveGame*, CurrentGameSave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadGameCompleteDelegate, UAstroShoppingSaveGame*, CurrentGameSave);

UCLASS()
class ASTROSHOPPING_API UPersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateGameSave();

	UFUNCTION(BlueprintCallable)
	void LoadGameSaveFromDisk(bool bAsync);

	UFUNCTION(BlueprintCallable)
	bool DoesGameSaveExistOnDisk();

	UFUNCTION(BlueprintCallable)
	void RequestSave(bool bAsync);

	UFUNCTION(BlueprintCallable)
	void RequestLoad(UObject* Requester);

	UPROPERTY(BlueprintAssignable)
	FSaveGameCompleteDelegate OnGameSaved;

	UPROPERTY(BlueprintAssignable)
	FLoadGameCompleteDelegate OnGameLoaded;
	
private:
	void WriteGameSaveToDisk(bool bAsync);

	UPROPERTY()
	TObjectPtr<class UAstroShoppingSaveGame> CurrentGameSave;

	FString SlotName = "GameSave";
	
};
