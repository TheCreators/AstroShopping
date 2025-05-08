#include "PersistenceSubsystem.h"
#include "AstroShoppingSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include "Saveable.h"

void UPersistenceSubsystem::CreateGameSave()
{
	CurrentGameSave = Cast<UAstroShoppingSaveGame>(UGameplayStatics::CreateSaveGameObject(UAstroShoppingSaveGame::StaticClass()));
	if (CurrentGameSave != nullptr)
	{
		OnGameLoaded.Broadcast(CurrentGameSave);
		OnGameLoadedNative.Broadcast(CurrentGameSave);
	}
}

void UPersistenceSubsystem::LoadGameSaveFromDisk(bool bAsync)
{
	if (!DoesGameSaveExistOnDisk())
	{
		UE_LOG(LogTemp, Warning, TEXT("No game save found on disk!"));
		return;
	}

	if (bAsync)
	{
		FAsyncLoadGameFromSlotDelegate LoadedDelegate;
		LoadedDelegate.BindLambda([this](const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
			{
				CurrentGameSave = Cast<UAstroShoppingSaveGame>(LoadedGameData);
				if (CurrentGameSave != nullptr)
				{
					OnGameLoaded.Broadcast(CurrentGameSave);
					OnGameLoadedNative.Broadcast(CurrentGameSave);
				}
			});
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, LoadedDelegate);
	}
	else
	{
		CurrentGameSave = Cast<UAstroShoppingSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentGameSave != nullptr)
		{
			OnGameLoaded.Broadcast(CurrentGameSave);
			OnGameLoadedNative.Broadcast(CurrentGameSave);
		}
	}
}

bool UPersistenceSubsystem::DoesGameSaveExistOnDisk()
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

void UPersistenceSubsystem::RequestSave(bool bAsync)
{
	TArray<AActor*> SaveableActors;
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid!"));
		return;
	}
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), SaveableActors);

	TArray<UObject*> SaveableObjects;
	
	SaveableObjects.Append(SaveableActors);

	for (AActor* Actor : SaveableActors)
	{
		SaveableObjects.Append(Actor->GetComponentsByInterface(USaveable::StaticClass()));
	}

	if (CurrentGameSave == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentGameSave is null!"));
		return;
	}

	CurrentGameSave->SaveAllData(SaveableObjects);

	WriteGameSaveToDisk(bAsync);
}

void UPersistenceSubsystem::RequestLoad(UObject* Requester)
{
	if (CurrentGameSave == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentGameSave is null!"));
		return;
	}

	CurrentGameSave->LoadDataForRequester(Requester);
}

void UPersistenceSubsystem::WriteGameSaveToDisk(bool bAsync)
{
	if (CurrentGameSave == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentGameSave is null!"));
		return;
	}

	if (bAsync)
	{
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		SavedDelegate.BindLambda([this](const FString& SlotName, const int32 UserIndex, bool bSuccess)
			{
				if (bSuccess)
				{
					OnGameSaved.Broadcast(CurrentGameSave);
					OnGameSavedNative.Broadcast(CurrentGameSave);
				}
			});
		UGameplayStatics::AsyncSaveGameToSlot(CurrentGameSave, SlotName, 0, SavedDelegate);
	}
	else
	{
		UGameplayStatics::SaveGameToSlot(CurrentGameSave, SlotName, 0);
		OnGameSaved.Broadcast(CurrentGameSave);
		OnGameSavedNative.Broadcast(CurrentGameSave);
	}
}
