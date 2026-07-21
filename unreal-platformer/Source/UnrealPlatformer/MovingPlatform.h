#pragma once

#include "CoreMinimal.h"
#include "PlatformBlock.h"
#include "MovingPlatform.generated.h"

/**
 * A platform block that glides between its spawn location and spawn location +
 * MoveOffset, easing at both ends. The character rides it automatically via the
 * character movement component's based-movement handling.
 */
UCLASS()
class UNREALPLATFORMER_API AMovingPlatform : public APlatformBlock
{
	GENERATED_BODY()

public:
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;

	/** Only meaningful before BeginPlay (i.e. between deferred spawn and FinishSpawning). */
	void SetMotion(const FVector& InMoveOffset, float InPeriod);

protected:
	virtual void BeginPlay() override;

private:
	/** Where the platform travels to, relative to its spawn location. */
	UPROPERTY(EditAnywhere, Category = "Platform")
	FVector MoveOffset = FVector(600.f, 0.f, 0.f);

	/** Seconds for a full out-and-back cycle. */
	UPROPERTY(EditAnywhere, Category = "Platform")
	float Period = 6.f;

	FVector StartLocation = FVector::ZeroVector;
	float RunningTime = 0.f;
};
