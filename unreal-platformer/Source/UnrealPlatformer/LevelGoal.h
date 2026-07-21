#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGoal.generated.h"

class UBoxComponent;
class URotatingMovementComponent;
class UStaticMeshComponent;

/** The slowly spinning gold pillar at the end of the course. Touch it to win. */
UCLASS()
class UNREALPLATFORMER_API ALevelGoal : public AActor
{
	GENERATED_BODY()

public:
	ALevelGoal();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Goal")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Goal")
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	UPROPERTY(VisibleAnywhere, Category = "Goal")
	TObjectPtr<URotatingMovementComponent> RotatingMovement;
};
