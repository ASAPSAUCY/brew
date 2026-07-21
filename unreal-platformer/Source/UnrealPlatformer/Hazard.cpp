#include "Hazard.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "UObject/ConstructorHelpers.h"

AHazard::AHazard()
{
	PrimaryActorTick.bCanEverTick = false;

	// Extent 50 = a 100-unit box, matching the engine cube so the actor's spawn
	// scale sizes the trigger and the visual identically.
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger.Get());
	Trigger->InitBoxExtent(FVector(50.f, 50.f, 50.f));
	Trigger->SetMobility(EComponentMobility::Movable);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	HazardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HazardMesh"));
	HazardMesh->SetupAttachment(Trigger.Get());
	HazardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		HazardMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		HazardMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void AHazard::BeginPlay()
{
	Super::BeginPlay();

	if (bIsKillVolume)
	{
		HazardMesh->SetVisibility(false);
		return;
	}

	if (UMaterialInstanceDynamic* Material = HazardMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.9f, 0.05f, 0.05f));
	}
}

void AHazard::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (APlatformerCharacter* Character = Cast<APlatformerCharacter>(OtherActor))
	{
		if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
		{
			GameMode->HandlePlayerDeath(Character);
		}
	}
}
