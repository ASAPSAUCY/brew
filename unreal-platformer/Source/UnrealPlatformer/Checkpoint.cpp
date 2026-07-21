#include "Checkpoint.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "UObject/ConstructorHelpers.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger.Get());
	Trigger->InitBoxExtent(FVector(60.f, 60.f, 120.f));
	Trigger->SetMobility(EComponentMobility::Movable);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	MarkerMesh->SetupAttachment(Trigger.Get());
	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 2.4f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		MarkerMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		MarkerMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	MarkerMaterial = MarkerMesh->CreateDynamicMaterialInstance(0);
	if (MarkerMaterial != nullptr)
	{
		MarkerMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.45f, 0.45f, 0.5f));
	}
}

void ACheckpoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bActivated || Cast<APlatformerCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	bActivated = true;

	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->SetCheckpoint(GetActorLocation(), GetActorRotation());
	}

	if (MarkerMaterial != nullptr)
	{
		MarkerMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.1f, 0.9f, 0.2f));
	}
}
