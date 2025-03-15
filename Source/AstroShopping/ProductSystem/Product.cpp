#include "Product.h"
#include "Net/UnrealNetwork.h"

AProduct::AProduct()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("Interactable"));
	Mesh->SetIsReplicated(true);

	Price = FMath::RandRange(50, 200);
}

void AProduct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProduct, Name);
	DOREPLIFETIME(AProduct, ID);
}

void AProduct::BeginPlay()
{
	Super::BeginPlay();
	
}

