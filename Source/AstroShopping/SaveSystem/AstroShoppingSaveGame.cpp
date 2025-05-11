#include "AstroShoppingSaveGame.h"
#include "Saveable.h"

UAstroShoppingSaveGame::UAstroShoppingSaveGame()
{
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Wheet"),
		25,
		TEXT("https://cdn-luma.com/imagine_3d_one/aec4320c-22d0-42e6-8278-28831858f45d/37086e3f42f0_Create_a_stylized_3D_wheat_b_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Water"),
		25,
		TEXT("https://cdn-luma.com/imagine_3d_one/7ab88286-0062-4237-b903-0369606013c8/7d4c99347318_water_bottle__3d_asset_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Lettuce"),
		50,
		TEXT("https://cdn-luma.com/imagine_3d_one/9aa519e9-8932-45c0-a17f-24063ddc8139/5b6e500d6919_Lettuce__3d_asset_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Eggs"),
		50,
		TEXT("https://cdn-luma.com/imagine_3d_one/10ee5bb2-7c54-4f30-b9bd-38c6b0f351c8/beb72f746c80_six_egg_carton_with_smooth_w_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Milk"),
		50,
		TEXT("https://cdn-luma.com/imagine_3d_one/8eb95072-8e96-4582-adb3-1a9e255db99e/6667e44b1e61_classic_rectangular_milk_car_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Plastic"),
		100,
		TEXT("https://cdn-luma.com/imagine_3d_one/dae90ffe-6bf7-4e0d-bcf7-87bab8d31048/279b108f4657_vibrant_red_plastic_interloc_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Wool"),
		100,
		TEXT("https://cdn-luma.com/imagine_3d_one/c9b57566-ecf3-4b5b-b11e-af73b1482495/4eb3f45a7cdf_loosely_wound_ball_of_wool_y_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Meat"),
		150,
		TEXT("https://cdn-luma.com/imagine_3d_one/5445cc6d-ee9b-40d6-9bde-4f24c68fabed/f2fd952e4299_flat_raw_ribeye_steak_showca_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Wood"),
		250,
		TEXT("https://cdn-luma.com/imagine_3d_one/273c6d08-9232-4a02-b824-79a7428a6e04/884e8214833f_logs__3d_asset_0_glb.glb")
	});
	ProductData.Add(FGuid::NewGuid(), {
		TEXT("Iron"),
		500,
		TEXT("https://cdn-luma.com/imagine_3d_one/b560809c-e434-43a2-ae57-ffbcce929ff4/071ee4ded217_light_metal_ingot__3d_asset_0_glb.glb")
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
