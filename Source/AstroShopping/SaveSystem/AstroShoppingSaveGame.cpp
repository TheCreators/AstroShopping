#include "AstroShoppingSaveGame.h"
#include "Saveable.h"

UAstroShoppingSaveGame::UAstroShoppingSaveGame()
{
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Test sign"),
		100,
		TEXT("https://cdn-luma.com/imagine_3d_one/4997791a-315c-4d24-ba68-40da419010b8/91fc50713cdb_Test_sign__3d_asset_0_glb.glb")
	});
}

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
