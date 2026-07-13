#include "CourseBuilder.h"

#include "Checkpoint.h"
#include "CollectibleCoin.h"
#include "Components/LightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Hazard.h"
#include "LevelGoal.h"
#include "MovingPlatform.h"
#include "PlatformBlock.h"
#include "PlatformerGameMode.h"

void ACourseBuilder::BuildCourse()
{
	const FLinearColor Slate(0.35f, 0.38f, 0.45f);
	const FLinearColor Teal(0.05f, 0.55f, 0.55f);
	const FLinearColor Orange(0.9f, 0.45f, 0.1f);
	const FLinearColor Gold(1.f, 0.75f, 0.15f);

	SpawnLighting();

	// ---- Section 1: start area -------------------------------------------------
	// 800x800x50 pad; its top surface sits at Z=25.
	SpawnBlock(FVector(0.f, 0.f, 0.f), FVector(8.f, 8.f, 0.5f), Slate);
	SpawnPlayerStart(FVector(0.f, 0.f, 130.f));
	SpawnCoin(FVector(250.f, 250.f, 115.f));
	SpawnCoin(FVector(-250.f, 250.f, 115.f));

	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->SetCheckpoint(FVector(0.f, 0.f, 130.f), FRotator::ZeroRotator);
	}

	// ---- Section 2: rising hops ------------------------------------------------
	SpawnBlock(FVector(700.f, 0.f, 50.f), FVector(3.f, 3.f, 0.5f), Teal);
	SpawnBlock(FVector(1300.f, 0.f, 150.f), FVector(3.f, 3.f, 0.5f), Teal);
	SpawnBlock(FVector(1900.f, 0.f, 250.f), FVector(3.f, 3.f, 0.5f), Teal);
	SpawnCoin(FVector(475.f, 0.f, 275.f));
	SpawnCoin(FVector(1000.f, 0.f, 325.f));
	SpawnCoin(FVector(1600.f, 0.f, 425.f));

	// ---- Section 3: horizontal moving platform over a long gap ------------------
	SpawnMovingPlatform(FVector(2500.f, 0.f, 200.f), FVector(2.5f, 2.5f, 0.4f), FVector(700.f, 0.f, 0.f), 6.f, Orange);
	SpawnCoinRow(FVector(2700.f, 0.f, 310.f), FVector(250.f, 0.f, 0.f), 3);

	// ---- Section 4: checkpoint platform with flanking hazards -------------------
	SpawnBlock(FVector(3900.f, 0.f, 250.f), FVector(5.f, 5.f, 0.5f), Slate);
	SpawnCheckpoint(FVector(3900.f, 0.f, 400.f));
	SpawnHazard(FVector(3900.f, 150.f, 325.f), FVector(1.f, 1.f, 1.f));
	SpawnHazard(FVector(3900.f, -150.f, 325.f), FVector(1.f, 1.f, 1.f));
	SpawnCoinRow(FVector(3750.f, 0.f, 365.f), FVector(150.f, 0.f, 0.f), 3);

	// ---- Section 5: side detour with bonus coins --------------------------------
	SpawnBlock(FVector(3900.f, 600.f, 300.f), FVector(2.5f, 2.5f, 0.4f), Teal);
	SpawnBlock(FVector(3900.f, 1150.f, 350.f), FVector(2.5f, 2.5f, 0.4f), Teal);
	SpawnCoin(FVector(3900.f, 600.f, 410.f));
	SpawnCoinRow(FVector(3900.f, 1050.f, 460.f), FVector(0.f, 100.f, 0.f), 3);

	// ---- Section 6: vertical moving platform up to the high ledge ---------------
	SpawnMovingPlatform(FVector(4550.f, 0.f, 250.f), FVector(2.5f, 2.5f, 0.4f), FVector(0.f, 0.f, 400.f), 5.f, Orange);
	SpawnBlock(FVector(5100.f, 0.f, 620.f), FVector(3.f, 3.f, 0.5f), Teal);
	SpawnCheckpoint(FVector(5100.f, 0.f, 770.f));

	// ---- Section 7: narrow bridge with a hazard to jump over --------------------
	SpawnBlock(FVector(5650.f, 0.f, 630.f), FVector(6.f, 1.f, 0.3f), Slate);
	SpawnHazard(FVector(5650.f, 0.f, 695.f), FVector(1.f, 1.f, 1.f));
	SpawnCoin(FVector(5450.f, 0.f, 735.f));
	SpawnCoin(FVector(5850.f, 0.f, 735.f));

	// ---- Section 8: goal platform ------------------------------------------------
	SpawnBlock(FVector(6350.f, 0.f, 650.f), FVector(5.f, 5.f, 0.5f), Gold);
	SpawnGoal(FVector(6350.f, 0.f, 800.f));
	SpawnCoin(FVector(6350.f, 200.f, 765.f));

	// ---- Safety net: giant invisible kill volume under everything ---------------
	SpawnKillVolume(FVector(3000.f, 0.f, -1100.f), FVector(90.f, 60.f, 4.f));
}

APlatformBlock* ACourseBuilder::SpawnBlock(const FVector& Center, const FVector& Scale, const FLinearColor& Color)
{
	const FTransform Transform(FRotator::ZeroRotator, Center, Scale);
	APlatformBlock* Block = GetWorld()->SpawnActorDeferred<APlatformBlock>(
		APlatformBlock::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Block != nullptr)
	{
		Block->SetBaseColor(Color);
		Block->FinishSpawning(Transform);
	}
	return Block;
}

void ACourseBuilder::SpawnMovingPlatform(const FVector& Center, const FVector& Scale, const FVector& MoveOffset, float Period, const FLinearColor& Color)
{
	const FTransform Transform(FRotator::ZeroRotator, Center, Scale);
	AMovingPlatform* Platform = GetWorld()->SpawnActorDeferred<AMovingPlatform>(
		AMovingPlatform::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Platform != nullptr)
	{
		Platform->SetBaseColor(Color);
		Platform->SetMotion(MoveOffset, Period);
		Platform->FinishSpawning(Transform);
	}
}

void ACourseBuilder::SpawnCoin(const FVector& Location)
{
	GetWorld()->SpawnActor<ACollectibleCoin>(ACollectibleCoin::StaticClass(), Location, FRotator::ZeroRotator);
}

void ACourseBuilder::SpawnCoinRow(const FVector& Start, const FVector& Step, int32 Count)
{
	for (int32 Index = 0; Index < Count; ++Index)
	{
		SpawnCoin(Start + Step * static_cast<float>(Index));
	}
}

void ACourseBuilder::SpawnHazard(const FVector& Center, const FVector& Scale)
{
	const FTransform Transform(FRotator::ZeroRotator, Center, Scale);
	AHazard* Hazard = GetWorld()->SpawnActorDeferred<AHazard>(
		AHazard::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Hazard != nullptr)
	{
		Hazard->FinishSpawning(Transform);
	}
}

void ACourseBuilder::SpawnKillVolume(const FVector& Center, const FVector& Scale)
{
	const FTransform Transform(FRotator::ZeroRotator, Center, Scale);
	AHazard* Hazard = GetWorld()->SpawnActorDeferred<AHazard>(
		AHazard::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Hazard != nullptr)
	{
		Hazard->SetIsKillVolume(true);
		Hazard->FinishSpawning(Transform);
	}
}

void ACourseBuilder::SpawnCheckpoint(const FVector& Location)
{
	GetWorld()->SpawnActor<ACheckpoint>(ACheckpoint::StaticClass(), Location, FRotator::ZeroRotator);
}

void ACourseBuilder::SpawnGoal(const FVector& Location)
{
	GetWorld()->SpawnActor<ALevelGoal>(ALevelGoal::StaticClass(), Location, FRotator::ZeroRotator);
}

void ACourseBuilder::SpawnPlayerStart(const FVector& Location)
{
	GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), Location, FRotator::ZeroRotator);
}

void ACourseBuilder::SpawnLighting()
{
	UWorld* World = GetWorld();

	// Runtime-spawned lights must be Movable; there is no baked lighting to fall
	// back on. The sun drives the sky atmosphere; the shadowless fill light makes
	// sure surfaces facing away from the sun never go fully black.
	ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 2000.f), FRotator(-50.f, 30.f, 0.f));
	if (Sun != nullptr)
	{
		if (ULightComponent* SunLight = Sun->GetLightComponent())
		{
			SunLight->SetMobility(EComponentMobility::Movable);
			SunLight->SetIntensity(6.f);
		}
	}

	ADirectionalLight* Fill = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 2000.f), FRotator(-30.f, 200.f, 0.f));
	if (Fill != nullptr)
	{
		if (ULightComponent* FillLight = Fill->GetLightComponent())
		{
			FillLight->SetMobility(EComponentMobility::Movable);
			FillLight->SetIntensity(1.5f);
			FillLight->SetCastShadows(false);
		}
	}

	ASkyLight* Sky = World->SpawnActor<ASkyLight>(
		ASkyLight::StaticClass(), FVector(0.f, 0.f, 2000.f), FRotator::ZeroRotator);
	if (Sky != nullptr)
	{
		if (USkyLightComponent* SkyComponent = Sky->GetLightComponent())
		{
			SkyComponent->SetMobility(EComponentMobility::Movable);
			SkyComponent->SetIntensity(1.f);
		}
	}

	// Gives the world a blue sky and horizon instead of a black void.
	World->SpawnActor<ASkyAtmosphere>(ASkyAtmosphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
}
