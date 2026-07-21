#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hazard.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * Touching this kills the player (costing a life and respawning them at the
 * last checkpoint). Visible red cubes act as spikes; with bIsKillVolume set the
 * mesh is hidden and the actor doubles as the giant catch-all below the course.
 */
UCLASS()
class UNREALPLATFORMER_API AHazard : public AActor
{
	GENERATED_BODY()

public:
	AHazard();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Only meaningful before BeginPlay (i.e. between deferred spawn and FinishSpawning). */
	void SetIsKillVolume(bool bInIsKillVolume) { bIsKillVolume = bInIsKillVolume; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Hazard")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Hazard")
	TObjectPtr<UStaticMeshComponent> HazardMesh;

	UPROPERTY(EditAnywhere, Category = "Hazard")
	bool bIsKillVolume = false;
};
