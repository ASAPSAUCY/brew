#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CourseBuilder.generated.h"

class APlatformBlock;

/**
 * Spawns the whole playable level - geometry, pickups, hazards, lights and the
 * PlayerStart - when the game mode initializes. The project ships no map
 * assets at all; this class IS the level.
 */
UCLASS()
class UNREALPLATFORMER_API ACourseBuilder : public AActor
{
	GENERATED_BODY()

public:
	void BuildCourse();

private:
	APlatformBlock* SpawnBlock(const FVector& Center, const FVector& Scale, const FLinearColor& Color);
	void SpawnMovingPlatform(const FVector& Center, const FVector& Scale, const FVector& MoveOffset, float Period, const FLinearColor& Color);
	void SpawnCoin(const FVector& Location);
	void SpawnCoinRow(const FVector& Start, const FVector& Step, int32 Count);
	void SpawnHazard(const FVector& Center, const FVector& Scale);
	void SpawnKillVolume(const FVector& Center, const FVector& Scale);
	void SpawnCheckpoint(const FVector& Location);
	void SpawnGoal(const FVector& Location);
	void SpawnPlayerStart(const FVector& Location);
	void SpawnLighting();
};
