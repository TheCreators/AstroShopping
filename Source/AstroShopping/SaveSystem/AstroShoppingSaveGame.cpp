#include "AstroShoppingSaveGame.h"
#include "Saveable.h"

void UAstroShoppingSaveGame::SaveAllData(TArray<UObject*> SaveableObjects)
{
	for (UObject* Object : SaveableObjects)
	{
		if (Object->Implements<USaveable>())
		{
			ISaveable::Execute_SaveData(Object, this);
		}
	}
}

void UAstroShoppingSaveGame::LoadDataForRequester(UObject* Requester)
{
	if (Requester->Implements<USaveable>())
	{
		ISaveable::Execute_LoadData(Requester, this);
	}
}
