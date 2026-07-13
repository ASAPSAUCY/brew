#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UBoxComponent;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/**
 * A gray pillar that turns green when the player runs through it and becomes
 * the new respawn point.
 */
UCLASS()
class UNREALPLATFORMER_API ACheckpoint : public AActor
{
	GENERATED_BODY()

public:
	ACheckpoint();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MarkerMaterial;

	bool bActivated = false;
};
