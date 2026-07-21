#include "MemoryCreature.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "MQGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StoryData.h"
#include "UObject/ConstructorHelpers.h"

AMemoryCreature::AMemoryCreature()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(InteractionSphere.Get());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TopperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopperMesh"));
	TopperMesh->SetupAttachment(BodyMesh.Get());
	TopperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		SphereAsset = SphereMesh.Object;
		TopperMesh->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		CylinderAsset = CylinderMesh.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		BodyMesh->SetMaterial(0, BaseMaterial.Object);
		TopperMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void AMemoryCreature::BeginPlay()
{
	Super::BeginPlay();

	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	if (!Creatures.IsValidIndex(CreatureIndex))
	{
		return;
	}
	const FMQCreatureDef& Def = Creatures[CreatureIndex];

	if (Def.Shape == EMQShape::Sphere && SphereAsset != nullptr)
	{
		BodyMesh->SetStaticMesh(SphereAsset);
	}
	else if (Def.Shape == EMQShape::Cylinder && CylinderAsset != nullptr)
	{
		BodyMesh->SetStaticMesh(CylinderAsset);
	}

	BodyBaseZ = 55.f * Def.Size;
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, BodyBaseZ));
	BodyMesh->SetRelativeScale3D(FVector(Def.Size, Def.Size, Def.Size));
	// Topper scale is relative to the body, so it stays proportional.
	TopperMesh->SetRelativeLocation(FVector(0.f, 0.f, 68.f));
	TopperMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

	if (UMaterialInstanceDynamic* BodyMaterial = BodyMesh->CreateDynamicMaterialInstance(0))
	{
		BodyMaterial->SetVectorParameterValue(TEXT("Color"), Def.BodyColor);
	}
	if (UMaterialInstanceDynamic* TopperMaterial = TopperMesh->CreateDynamicMaterialInstance(0))
	{
		TopperMaterial->SetVectorParameterValue(TEXT("Color"), Def.TopColor);
	}

	SetLabel(Def.Name);
	SetPrompt(FString::Printf(TEXT("E - Say hi to %s (wild memory!)"), *Def.Name));

	HomeLocation = GetActorLocation();
	WanderTarget = HomeLocation;
	BobPhase = static_cast<float>(CreatureIndex) * 1.3f;
	PauseTimer = FMath::FRandRange(0.f, 2.f);
}

void AMemoryCreature::PickNewWanderTarget()
{
	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	const float Radius = Creatures.IsValidIndex(CreatureIndex) ? Creatures[CreatureIndex].WanderRadius : 400.f;

	WanderTarget = HomeLocation + FVector(FMath::FRandRange(-Radius, Radius), FMath::FRandRange(-Radius, Radius), 0.f);
}

void AMemoryCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Idle bob, always.
	const float Time = GetWorld()->GetTimeSeconds();
	FVector BodyLocation = BodyMesh->GetRelativeLocation();
	BodyLocation.Z = BodyBaseZ + 8.f * FMath::Sin(Time * 2.f + BobPhase);
	BodyMesh->SetRelativeLocation(BodyLocation);

	if (bBattling)
	{
		return;
	}

	if (PauseTimer > 0.f)
	{
		PauseTimer -= DeltaTime;
		if (PauseTimer <= 0.f)
		{
			PickNewWanderTarget();
		}
		return;
	}

	FVector ToTarget = WanderTarget - GetActorLocation();
	ToTarget.Z = 0.f;
	const float Distance = ToTarget.Size();
	if (Distance < 50.f)
	{
		PauseTimer = FMath::FRandRange(1.f, 3.f);
		return;
	}

	const FVector Step = ToTarget.GetSafeNormal() * FMath::Min(150.f * DeltaTime, Distance);
	SetActorLocation(GetActorLocation() + Step);
	SetActorRotation(FRotator(0.f, ToTarget.Rotation().Yaw, 0.f));
	FaceLabelToCamera();
}

void AMemoryCreature::Interact(AMQCharacter* Player)
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->StartBattle(this);
	}
}
