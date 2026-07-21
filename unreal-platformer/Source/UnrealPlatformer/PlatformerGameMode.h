#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerGameMode.generated.h"

class APlatformerCharacter;

UENUM()
enum class EPlatformerGameState : uint8
{
	Playing,
	Won,
	Lost
};

/**
 * Owns the run: score, lives, checkpoints, win/lose state. Also bootstraps the
 * whole level - the project ships no map assets, so InitGame spawns a
 * CourseBuilder that constructs every platform, pickup and light in code.
 */
UCLASS()
class UNREALPLATFORMER_API APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Called by each coin when it enters play, so the HUD can show a grand total. */
	void RegisterCoin();

	/** Called by a coin when the player collects it. */
	void AddCoin();

	/** Updates where the player respawns after dying. */
	void SetCheckpoint(const FVector& Location, const FRotator& Rotation);

	void HandlePlayerDeath(APlatformerCharacter* Character);
	void HandleGoalReached();

	/** Reloads the level from scratch. Bound to the R key. */
	void RestartLevel();

	int32 GetScore() const { return Score; }
	int32 GetTotalCoins() const { return TotalCoins; }
	int32 GetLives() const { return Lives; }
	EPlatformerGameState GetPlatformerState() const { return PlatformerState; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Platformer")
	int32 Score = 0;

	UPROPERTY(VisibleAnywhere, Category = "Platformer")
	int32 TotalCoins = 0;

	UPROPERTY(VisibleAnywhere, Category = "Platformer")
	int32 Lives = 3;

	UPROPERTY(VisibleAnywhere, Category = "Platformer")
	EPlatformerGameState PlatformerState = EPlatformerGameState::Playing;

	FVector CheckpointLocation = FVector::ZeroVector;
	FRotator CheckpointRotation = FRotator::ZeroRotator;
};
