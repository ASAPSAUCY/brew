#pragma once

#include "CoreMinimal.h"
#include "MQInteractable.h"
#include "DogActors.generated.h"

class UStaticMeshComponent;

/**
 * Buddy - Kyle's dog. Trots after the player everywhere, and has opinions.
 */
UCLASS()
class MEMORYQUEST_API ACompanionDog : public AMQInteractable
{
	GENERATED_BODY()

public:
	ACompanionDog();

	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Dog")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Dog")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, Category = "Dog")
	TObjectPtr<UStaticMeshComponent> TailMesh;

	/** Fill color applied to the whole dog in BeginPlay. */
	FLinearColor CoatColor = FLinearColor(0.85f, 0.60f, 0.20f);

	float BobPhase = 0.f;
};

/**
 * Rumor - Claudia's dog, who passed away last year. A gentle glowing spirit
 * that waits at his memorial tree in Cozy Park. Sitting with him adds him to
 * the Memorydex as its one special entry.
 */
UCLASS()
class MEMORYQUEST_API ARumorSpirit : public ACompanionDog
{
	GENERATED_BODY()

public:
	ARumorSpirit();

	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

private:
	bool bRemembered = false;
};
