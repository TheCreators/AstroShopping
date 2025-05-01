#include "BotSpawner.h"

ABotSpawner::ABotSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABotSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	SpawnBot();
}

void ABotSpawner::ScheduleNextSpawn()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World context is missing in ScheduleNextSpawn"));
		return;
	}

	float SpawnDelay = BaseSpawnDelay / GetAdsConversionRate();

	FTimerHandle SpawnTimerHandle;

	World->GetTimerManager().SetTimer(
		SpawnTimerHandle, 
		this, 
		&ABotSpawner::SpawnBot, 
		SpawnDelay, 
		false
	);
}

void ABotSpawner::SpawnBot()
{
	if (!BotClass) {
		UE_LOG(LogTemp, Error, TEXT("Bot class not set"));
		return;
	}

	int32 MaxBots = FMath::CeilToInt(FMath::Pow(BaseMaxBots, GetAdsConversionRate()));

	if (CurrentBotCount >= MaxBots) {
		UE_LOG(LogTemp, Warning, TEXT("Max bot count reached"));
		ScheduleNextSpawn();
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World context is missing for bot spawner"));
		return;
	}

	APawn* SpawnedBot = World->SpawnActor<APawn>(
		BotClass,
		GetActorLocation(),
		GetActorRotation()
	);

	if (!SpawnedBot)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn bot"));
		return;
	}

	CurrentBotCount++;
	SpawnedBot->OnDestroyed.AddDynamic(this, &ABotSpawner::OnBotDestroyed);

	ScheduleNextSpawn();
}

void ABotSpawner::OnBotDestroyed(AActor* DestroyedActor)
{
	--CurrentBotCount;
}
