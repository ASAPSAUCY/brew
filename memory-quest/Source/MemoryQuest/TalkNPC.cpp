#include "TalkNPC.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "MQCharacter.h"
#include "MQGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StoryData.h"
#include "UObject/ConstructorHelpers.h"

ATalkNPC::ATalkNPC()
{
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(InteractionSphere);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 55.f));
	BodyMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 1.1f));

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(InteractionSphere);
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetRelativeLocation(FVector(0.f, 0.f, 138.f));
	HeadMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		HeadMesh->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		BodyMesh->SetMaterial(0, BaseMaterial.Object);
		HeadMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void ATalkNPC::BeginPlay()
{
	Super::BeginPlay();

	const TArray<FMQNpcDef>& Npcs = StoryData::Npcs();
	if (!Npcs.IsValidIndex(NpcIndex))
	{
		return;
	}
	const FMQNpcDef& Def = Npcs[NpcIndex];

	if (UMaterialInstanceDynamic* BodyMaterial = BodyMesh->CreateDynamicMaterialInstance(0))
	{
		BodyMaterial->SetVectorParameterValue(TEXT("Color"), Def.Color);
	}
	if (UMaterialInstanceDynamic* HeadMaterial = HeadMesh->CreateDynamicMaterialInstance(0))
	{
		// Heads get a lighter tint of the body color so faces read at a glance.
		HeadMaterial->SetVectorParameterValue(TEXT("Color"), Def.Color * 0.5f + FLinearColor(0.5f, 0.45f, 0.4f));
	}

	SetLabel(Def.Name);
	SetPrompt(FString::Printf(TEXT("E - Talk to %s"), *Def.Name));
	SetActorRotation(FRotator(0.f, Def.FacingYaw, 0.f));
	FaceLabelToCamera();
}

void ATalkNPC::Interact(AMQCharacter* Player)
{
	const TArray<FMQNpcDef>& Npcs = StoryData::Npcs();
	if (!Npcs.IsValidIndex(NpcIndex))
	{
		return;
	}
	const FMQNpcDef& Def = Npcs[NpcIndex];

	if (Player != nullptr)
	{
		const FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
		SetActorRotation(FRotator(0.f, ToPlayer.Rotation().Yaw, 0.f));
		FaceLabelToCamera();
	}

	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->StartDialogue(Def.Name, Def.Pages, Def.Color);
	}
}
