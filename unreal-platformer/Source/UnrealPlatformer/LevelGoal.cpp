#include "LevelGoal.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "UObject/ConstructorHelpers.h"

ALevelGoal::ALevelGoal()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->InitBoxExtent(FVector(80.f, 80.f, 120.f));
	Trigger->SetMobility(EComponentMobility::Movable);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	PillarMesh->SetupAttachment(Trigger);
	PillarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PillarMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 3.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		PillarMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		PillarMesh->SetMaterial(0, BaseMaterial.Object);
	}

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 90.f, 0.f);
}

void ALevelGoal::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* Material = PillarMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.75f, 0.1f));
	}
}

void ALevelGoal::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Cast<APlatformerCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->HandleGoalReached();
	}
}
