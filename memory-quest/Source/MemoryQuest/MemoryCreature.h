#pragma once

#include "CoreMinimal.h"
#include "MQInteractable.h"
#include "MemoryCreature.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

/**
 * A wild memory wandering its home zone. Walk up and press E to start a
 * (gentle) battle; befriend it with a treat and it joins the Memorydex.
 * Content comes from StoryData::Creatures().
 */
UCLASS()
class MEMORYQUEST_API AMemoryCreature : public AMQInteractable
{
	GENERATED_BODY()

public:
	AMemoryCreature();

	virtual void Tick(float DeltaTime) override;

	/** Which StoryData::Creatures() entry this is. Set before FinishSpawning. */
	void SetCreatureIndex(int32 InIndex) { CreatureIndex = InIndex; }
	int32 GetCreatureIndex() const { return CreatureIndex; }

	void SetInBattle(bool bInBattle) { bBattling = bInBattle; }

	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

private:
	void PickNewWanderTarget();

	UPROPERTY(VisibleAnywhere, Category = "Creature")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Creature")
	TObjectPtr<UStaticMeshComponent> TopperMesh;

	UPROPERTY(VisibleAnywhere, Category = "Creature")
	TObjectPtr<UStaticMeshComponent> LeftEye;

	UPROPERTY(VisibleAnywhere, Category = "Creature")
	TObjectPtr<UStaticMeshComponent> RightEye;

	UPROPERTY(VisibleAnywhere, Category = "Creature")
	TObjectPtr<UStaticMeshComponent> LeftPupil;

	UPROPERTY(VisibleAnywhere, Category = "Creature")
	TObjectPtr<UStaticMeshComponent> RightPupil;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereAsset;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderAsset;

	int32 CreatureIndex = 0;
	bool bBattling = false;

	FVector HomeLocation = FVector::ZeroVector;
	FVector WanderTarget = FVector::ZeroVector;
	float PauseTimer = 0.f;
	float BobPhase = 0.f;
	float BodyBaseZ = 45.f;
};
