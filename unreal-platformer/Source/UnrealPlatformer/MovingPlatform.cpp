#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingPlatform::SetMotion(const FVector& InMoveOffset, float InPeriod)
{
	MoveOffset = InMoveOffset;
	Period = FMath::Max(InPeriod, 0.1f);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	// 0 -> 1 -> 0 over one period, easing in and out at both ends of the run.
	const float Alpha = 0.5f - 0.5f * FMath::Cos(2.f * PI * RunningTime / Period);
	SetActorLocation(StartLocation + MoveOffset * Alpha);
}
