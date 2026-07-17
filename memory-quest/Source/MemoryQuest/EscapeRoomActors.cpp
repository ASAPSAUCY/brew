#include "EscapeRoomActors.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "MQGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StoryData.h"
#include "UObject/ConstructorHelpers.h"

// ---------------------------------------------------------------------------
// AEscapeDoor
// ---------------------------------------------------------------------------

AEscapeDoor::AEscapeDoor()
{
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(InteractionSphere);
	DoorMesh->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
	DoorMesh->SetRelativeScale3D(FVector(2.6f, 0.35f, 2.6f));
	// The door physically blocks the entrance until it opens.
	DoorMesh->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		DoorMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void AEscapeDoor::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* Material = DoorMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.70f, 0.10f, 0.12f));
	}

	SetLabel(TEXT("The Final Room"));
}

FString AEscapeDoor::GetPrompt() const
{
	const AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>();
	if (GameMode == nullptr)
	{
		return FString();
	}

	if (GameMode->IsEscapeRoomUnlocked())
	{
		return TEXT("E - Open The Final Room. It's time.");
	}

	return FString::Printf(TEXT("Locked - memories %d/%d, keepsakes %d/%d"),
		GameMode->GetCaughtCount(), StoryData::CreaturesNeededForEscapeRoom(),
		GameMode->GetKeepsakeCount(), StoryData::KeepsakesNeededForEscapeRoom());
}

void AEscapeDoor::Interact(AMQCharacter* Player)
{
	AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>();
	if (GameMode == nullptr)
	{
		return;
	}

	if (GameMode->IsEscapeRoomUnlocked())
	{
		GameMode->OnEscapeDoorOpened();
		Destroy();
		return;
	}

	const TArray<FString> Pages = {
		FString::Printf(TEXT("The door doesn't budge. It wants more of your story first: %d/%d memories caught, %d/%d keepsakes found."),
			GameMode->GetCaughtCount(), StoryData::CreaturesNeededForEscapeRoom(),
			GameMode->GetKeepsakeCount(), StoryData::KeepsakesNeededForEscapeRoom()),
		TEXT("(A faint smell of mimosa drifts under the door. Judging you.)")
	};
	GameMode->StartDialogue(TEXT("The Final Room"), Pages, FLinearColor(0.70f, 0.10f, 0.12f));
}

// ---------------------------------------------------------------------------
// APuzzlePedestal
// ---------------------------------------------------------------------------

APuzzlePedestal::APuzzlePedestal()
{
	ColumnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ColumnMesh"));
	ColumnMesh->SetupAttachment(InteractionSphere);
	ColumnMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ColumnMesh->SetRelativeLocation(FVector(0.f, 0.f, 45.f));
	ColumnMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.9f));

	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	OrbMesh->SetupAttachment(InteractionSphere);
	OrbMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OrbMesh->SetRelativeLocation(FVector(0.f, 0.f, 115.f));
	OrbMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		ColumnMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		OrbMesh->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		ColumnMesh->SetMaterial(0, BaseMaterial.Object);
		OrbMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void APuzzlePedestal::BeginPlay()
{
	Super::BeginPlay();

	switch (PuzzleIndex)
	{
	case 0:  OrbColor = FLinearColor(0.95f, 0.55f, 0.15f); break; // pumpkin
	case 1:  OrbColor = FLinearColor(0.80f, 0.35f, 0.10f); break; // basketball
	default: OrbColor = FLinearColor(0.95f, 0.85f, 0.15f); break; // taxi
	}

	if (UMaterialInstanceDynamic* ColumnMaterial = ColumnMesh->CreateDynamicMaterialInstance(0))
	{
		ColumnMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.30f, 0.28f, 0.35f));
	}

	OrbMaterial = OrbMesh->CreateDynamicMaterialInstance(0);
	if (OrbMaterial != nullptr)
	{
		OrbMaterial->SetVectorParameterValue(TEXT("Color"), OrbColor);
	}

	const TArray<FString>& Clues = StoryData::PuzzleClues();
	SetPrompt(FString::Printf(TEXT("E - Press the pedestal. %s"),
		Clues.IsValidIndex(PuzzleIndex) ? *Clues[PuzzleIndex] : TEXT("")));
}

void APuzzlePedestal::Interact(AMQCharacter* Player)
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->ActivatePedestal(this);
	}
}

void APuzzlePedestal::MarkSolved()
{
	if (OrbMaterial != nullptr)
	{
		OrbMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.10f, 0.90f, 0.20f));
	}
}

void APuzzlePedestal::ResetPedestal()
{
	if (OrbMaterial != nullptr)
	{
		OrbMaterial->SetVectorParameterValue(TEXT("Color"), OrbColor);
	}
}

// ---------------------------------------------------------------------------
// ALevelTrophy
// ---------------------------------------------------------------------------

ALevelTrophy::ALevelTrophy()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(InteractionSphere);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
	BaseMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));

	CupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CupMesh"));
	CupMesh->SetupAttachment(InteractionSphere);
	CupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CupMesh->SetRelativeLocation(FVector(0.f, 0.f, 95.f));
	CupMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		BaseMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		CupMesh->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		BaseMesh->SetMaterial(0, BaseMaterial.Object);
		CupMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void ALevelTrophy::BeginPlay()
{
	Super::BeginPlay();

	const FLinearColor Gold(1.f, 0.78f, 0.15f);
	if (UMaterialInstanceDynamic* Material = BaseMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), Gold);
	}
	if (UMaterialInstanceDynamic* Material = CupMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), Gold);
	}

	SetLabel(TEXT("Four Years & Counting"));
	SetPrompt(TEXT("E - Finish what you started"));
}

void ALevelTrophy::Interact(AMQCharacter* Player)
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->StartFinale();
	}
}
