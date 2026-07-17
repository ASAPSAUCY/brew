#include "DogActors.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MQGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StoryData.h"
#include "UObject/ConstructorHelpers.h"

// ---------------------------------------------------------------------------
// ACompanionDog (Buddy)
// ---------------------------------------------------------------------------

ACompanionDog::ACompanionDog()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(InteractionSphere);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 45.f));
	BodyMesh->SetRelativeScale3D(FVector(0.9f, 0.45f, 0.5f));

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(InteractionSphere);
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetRelativeLocation(FVector(55.f, 0.f, 82.f));
	HeadMesh->SetRelativeScale3D(FVector(0.34f, 0.34f, 0.34f));

	TailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TailMesh"));
	TailMesh->SetupAttachment(InteractionSphere);
	TailMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TailMesh->SetRelativeLocation(FVector(-55.f, 0.f, 72.f));
	TailMesh->SetRelativeScale3D(FVector(0.14f, 0.14f, 0.4f));
	TailMesh->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
		HeadMesh->SetStaticMesh(CubeMesh.Object);
		TailMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		BodyMesh->SetMaterial(0, BaseMaterial.Object);
		HeadMesh->SetMaterial(0, BaseMaterial.Object);
		TailMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void ACompanionDog::BeginPlay()
{
	Super::BeginPlay();

	for (UStaticMeshComponent* Mesh : { BodyMesh.Get(), HeadMesh.Get(), TailMesh.Get() })
	{
		if (UMaterialInstanceDynamic* Material = Mesh->CreateDynamicMaterialInstance(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), CoatColor);
		}
	}

	SetLabel(StoryData::BuddyName());
	SetPrompt(FString::Printf(TEXT("E - Pet %s"), *StoryData::BuddyName()));
}

void ACompanionDog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (Player == nullptr)
	{
		return;
	}

	const float Time = GetWorld()->GetTimeSeconds();
	const FVector PlayerLocation = Player->GetActorLocation();
	FVector MyLocation = GetActorLocation();

	FVector ToPlayer = PlayerLocation - MyLocation;
	ToPlayer.Z = 0.f;
	const float Distance = ToPlayer.Size();

	if (Distance > 260.f)
	{
		const float Speed = Distance > 900.f ? 950.f : 520.f;
		const FVector Step = ToPlayer.GetSafeNormal() * FMath::Min(Speed * DeltaTime, Distance - 240.f);
		MyLocation += Step;
		SetActorRotation(FRotator(0.f, ToPlayer.Rotation().Yaw, 0.f));
	}

	// Ride at the player's ground level, with a happy trot-bob.
	MyLocation.Z = PlayerLocation.Z - 94.f + FMath::Abs(6.f * FMath::Sin(Time * 6.f + BobPhase));
	SetActorLocation(MyLocation);

	// Tail wag.
	TailMesh->SetRelativeRotation(FRotator(-30.f, 25.f * FMath::Sin(Time * 8.f), 0.f));
	FaceLabelToCamera();
}

void ACompanionDog::Interact(AMQCharacter* Player)
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->StartDialogue(StoryData::BuddyName(), StoryData::BuddyPages(), CoatColor);
	}
}

// ---------------------------------------------------------------------------
// ARumorSpirit
// ---------------------------------------------------------------------------

ARumorSpirit::ARumorSpirit()
{
	CoatColor = FLinearColor(0.85f, 0.95f, 1.f);
}

void ARumorSpirit::BeginPlay()
{
	// Deliberately skipping ACompanionDog::BeginPlay - Rumor has his own label,
	// prompt and coloring. AMQInteractable::BeginPlay is what we want.
	AMQInteractable::BeginPlay();

	for (UStaticMeshComponent* Mesh : { BodyMesh.Get(), HeadMesh.Get(), TailMesh.Get() })
	{
		if (UMaterialInstanceDynamic* Material = Mesh->CreateDynamicMaterialInstance(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), CoatColor);
		}
	}

	SetLabel(TEXT("Rumor"));
	SetPrompt(TEXT("E - Sit with Rumor for a moment"));
}

void ARumorSpirit::Tick(float DeltaTime)
{
	// Skip the follow behavior entirely; Rumor floats gently at his tree.
	AMQInteractable::Tick(DeltaTime);

	const float Time = GetWorld()->GetTimeSeconds();
	FVector BodyLocation = BodyMesh->GetRelativeLocation();
	BodyLocation.Z = 55.f + 10.f * FMath::Sin(Time * 1.2f);
	BodyMesh->SetRelativeLocation(BodyLocation);

	FVector HeadLocation = HeadMesh->GetRelativeLocation();
	HeadLocation.Z = 92.f + 10.f * FMath::Sin(Time * 1.2f);
	HeadMesh->SetRelativeLocation(HeadLocation);

	FVector TailLocation = TailMesh->GetRelativeLocation();
	TailLocation.Z = 82.f + 10.f * FMath::Sin(Time * 1.2f);
	TailMesh->SetRelativeLocation(TailLocation);
}

void ARumorSpirit::Interact(AMQCharacter* Player)
{
	if (bRemembered)
	{
		return;
	}
	bRemembered = true;

	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->RememberRumor();
	}

	SetPrompt(TEXT("Rumor is at peace. (Good boy.)"));
}
