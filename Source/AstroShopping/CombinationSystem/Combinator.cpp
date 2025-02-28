#include "Combinator.h"
#include "Net/UnrealNetwork.h"
#include <ctime>
#include "glTFRuntimeFunctionLibrary.h"

ACombinator::ACombinator()
{
	srand(static_cast<unsigned int>(time(0)));

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ACombinator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACombinator, ModelUrls);
	DOREPLIFETIME(ACombinator, bIsCombining);
}

void ACombinator::Server_StartCombination_Implementation(const FString& FirstProductName, const FString& SecondProductName)
{
	bIsCombining = true;

	AsyncTask(ENamedThreads::AnyThread, [this]()
		{
			//FPlatformProcess::Sleep(3.0);

			TArray<FString> NewModelUrls = {
				"https://cdn-luma.com/blender_convert/c6bb0fc8-8dab-4296-9663-962541b1bd98/cf23404dac6a_a08730d30c54_a_diamond_blue_Crys.glb",
				"https://cdn-luma.com/imagine_3d_one/fee121c7-63b9-4b2f-91cd-a33218a21ac1/18119caf96ee_Test_sign__3d_asset_0_glb.glb",
				"https://cdn-luma.com/imagine_3d_one/4997791a-315c-4d24-ba68-40da419010b8/91fc50713cdb_Test_sign__3d_asset_0_glb.glb"
			};

			int RandomIndex = rand() % NewModelUrls.Num();

			FString Url = NewModelUrls[RandomIndex];

			AsyncTask(ENamedThreads::GameThread, [this, Url]()
				{
					ModelUrls.Add(Url);
					OnRep_ModelUrls();
				});
		});
}

void ACombinator::OnRep_ModelUrls()
{
	FString ModelUrl = ModelUrls.Last();

	TMap<FString, FString> Headers;
	FglTFRuntimeConfig LoaderConfig;
	FglTFRuntimeHttpResponse ResponseDelegate;
	ResponseDelegate.BindDynamic(this, &ACombinator::OnModelDownloaded);

	UglTFRuntimeFunctionLibrary::glTFLoadAssetFromUrl(ModelUrl, Headers, ResponseDelegate, LoaderConfig);
}

void ACombinator::OnModelDownloaded(UglTFRuntimeAsset* Asset)
{
	if (Asset == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load model asset"));
		return;
	}

	FglTFRuntimeStaticMeshConfig StaticMeshConfig;
	StaticMeshConfig.bBuildSimpleCollision = true;

	FglTFRuntimeStaticMeshAsync Delegate;
	Delegate.BindDynamic(this, &ACombinator::OnModelLoaded);

	Asset->LoadStaticMeshAsync(0, Delegate, StaticMeshConfig);
}

void ACombinator::OnModelLoaded(UStaticMesh* StaticMesh)
{
	OnCombinationComplete.Broadcast(StaticMesh);
	bIsCombining = false;
}
