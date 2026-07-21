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
	BodyMesh->SetupAttachment(InteractionSphere.Get());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 55.f));
	BodyMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 1.1f));

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(InteractionSphere.Get());
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

	auto MakeFacePart = [this](const TCHAR* Name, const FVector& Location, float Scale) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Part->SetupAttachment(HeadMesh.Get());
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetRelativeLocation(Location);
		Part->SetRelativeScale3D(FVector(Scale, Scale, Scale));
		if (SphereMesh.Succeeded())
		{
			Part->SetStaticMesh(SphereMesh.Object);
		}
		if (BaseMaterial.Succeeded())
		{
			Part->SetMaterial(0, BaseMaterial.Object);
		}
		return Part;
	};
	LeftEye = MakeFacePart(TEXT("LeftEye"), FVector(44.f, -20.f, 8.f), 0.24f);
	RightEye = MakeFacePart(TEXT("RightEye"), FVector(44.f, 20.f, 8.f), 0.24f);
	LeftPupil = MakeFacePart(TEXT("LeftPupil"), FVector(52.f, -20.f, 8.f), 0.12f);
	RightPupil = MakeFacePart(TEXT("RightPupil"), FVector(52.f, 20.f, 8.f), 0.12f);

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

	for (UStaticMeshComponent* Eye : { LeftEye.Get(), RightEye.Get() })
	{
		if (UMaterialInstanceDynamic* Material = Eye->CreateDynamicMaterialInstance(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.97f, 0.97f, 0.95f));
		}
	}
	for (UStaticMeshComponent* Pupil : { LeftPupil.Get(), RightPupil.Get() })
	{
		if (UMaterialInstanceDynamic* Material = Pupil->CreateDynamicMaterialInstance(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.04f, 0.04f, 0.05f));
		}
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
