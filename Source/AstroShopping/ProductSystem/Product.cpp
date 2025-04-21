#include "Product.h"
#include "Net/UnrealNetwork.h"

AProduct::AProduct()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("Interactable"));
	SetRootComponent(Mesh);

	Price = FMath::RandRange(50, 200);
}

void AProduct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProduct, Name);
	DOREPLIFETIME(AProduct, ProductDataID);
	DOREPLIFETIME(AProduct, Price);
	DOREPLIFETIME(AProduct, Quantity);
}

void AProduct::BeginPlay()
{
	Super::BeginPlay();
	
}

