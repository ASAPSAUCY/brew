#include "PlatformerGameMode.h"

#include "CourseBuilder.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PlatformerCharacter.h"
#include "PlatformerHUD.h"

APlatformerGameMode::APlatformerGameMode()
{
	DefaultPawnClass = APlatformerCharacter::StaticClass();
	HUDClass = APlatformerHUD::StaticClass();
}

void APlatformerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Build the whole course now: InitGame runs before the player logs in, so the
	// PlayerStart spawned by the builder exists by the time RestartPlayer looks for one.
	if (ACourseBuilder* Builder = GetWorld()->SpawnActor<ACourseBuilder>())
	{
		Builder->BuildCourse();
	}
}

void APlatformerGameMode::RegisterCoin()
{
	++TotalCoins;
}

void APlatformerGameMode::AddCoin()
{
	if (PlatformerState == EPlatformerGameState::Playing)
	{
		++Score;
	}
}

void APlatformerGameMode::SetCheckpoint(const FVector& Location, const FRotator& Rotation)
{
	CheckpointLocation = Location;
	CheckpointRotation = Rotation;
}

void APlatformerGameMode::HandlePlayerDeath(APlatformerCharacter* Character)
{
	if (PlatformerState != EPlatformerGameState::Playing || Character == nullptr)
	{
		return;
	}

	--Lives;
	if (Lives > 0)
	{
		Character->MoveToRespawnPoint(CheckpointLocation + FVector(0.f, 0.f, 150.f), CheckpointRotation);
	}
	else
	{
		PlatformerState = EPlatformerGameState::Lost;
		Character->DisableGameplayInput();
	}
}

void APlatformerGameMode::HandleGoalReached()
{
	if (PlatformerState != EPlatformerGameState::Playing)
	{
		return;
	}

	PlatformerState = EPlatformerGameState::Won;

	if (APlatformerCharacter* Character = Cast<APlatformerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Character->DisableGameplayInput();
	}
}

void APlatformerGameMode::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("/Engine/Maps/Entry")));
}
