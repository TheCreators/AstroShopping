#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstroShopping/InteractionSystem/Interactable.h"
#include "Product.generated.h"

UCLASS()
class ASTROSHOPPING_API AProduct : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AProduct();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FGuid ProductDataID;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	TSoftObjectPtr<UStaticMesh> ProductMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	TSoftObjectPtr<UTexture2D> ProductThumbnail;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	int32 Price;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	int32 Quantity;

protected:
	virtual void BeginPlay() override;
};
