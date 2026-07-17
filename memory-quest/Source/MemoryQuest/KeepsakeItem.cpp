#include "KeepsakeItem.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "MQGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StoryData.h"
#include "UObject/ConstructorHelpers.h"

AKeepsakeItem::AKeepsakeItem()
{
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(InteractionSphere);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetRelativeLocation(FVector(0.f, 0.f, 45.f));
	ItemMesh->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		ItemMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		ItemMesh->SetMaterial(0, BaseMaterial.Object);
	}

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 120.f, 0.f);
}

void AKeepsakeItem::BeginPlay()
{
	Super::BeginPlay();

	const TArray<FMQKeepsakeDef>& Keepsakes = StoryData::Keepsakes();
	if (!Keepsakes.IsValidIndex(KeepsakeIndex))
	{
		return;
	}
	const FMQKeepsakeDef& Def = Keepsakes[KeepsakeIndex];

	if (UMaterialInstanceDynamic* Material = ItemMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), Def.Color);
	}

	SetLabel(Def.Name);
	SetPrompt(FString::Printf(TEXT("E - Pick up the %s"), *Def.Name));
}

void AKeepsakeItem::Interact(AMQCharacter* Player)
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->CollectKeepsake(KeepsakeIndex);
	}

	Destroy();
}
