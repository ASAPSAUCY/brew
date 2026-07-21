#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoryData.h"
#include "WorldBuilder.generated.h"

class AColorBlock;

/**
 * Spawns the entire island - grounds, buildings, props, lights, NPCs,
 * creatures, keepsakes, the escape room and the PlayerStart - when the game
 * mode initializes. The project ships no map assets; this class IS the map.
 *
 * Layout (world units):
 *   The Apartment (hub)  at (    0,     0)
 *   Hallow Grove         at (-6000, -6000)
 *   The Courts           at ( 6000, -6000)
 *   Cozy Park            at (-6000,  6000), Montreal Corner at (-7300, 7300)
 *   Little New York      at ( 6000,  6000)
 */
UCLASS()
class MEMORYQUEST_API AWorldBuilder : public AActor
{
	GENERATED_BODY()

public:
	void BuildWorld();

private:
	AColorBlock* Block(const FVector& Center, const FVector& Scale, const FLinearColor& Color, EMQShape Shape = EMQShape::Cube, const FRotator& Rotation = FRotator::ZeroRotator);
	void Zone(const FVector& Center, const FVector& Scale, const FString& Name, const FString& Subtitle);
	void Tree(const FVector& Base, const FLinearColor& CrownColor, float CrownScale = 2.2f);
	void Building(const FVector& Center, float Width, float Depth, float Height, const FLinearColor& WallColor, const FLinearColor& RoofColor, float DoorHalfWidth, bool bWithRoof);

	void BuildGrounds();
	void BuildHub();
	void BuildHallowGrove();
	void BuildCourts();
	void BuildLittleNewYork();
	void BuildCozyPark();
	void BuildEscapeRoom();
	void SpawnStoryActors();
	void SpawnLighting();
};
