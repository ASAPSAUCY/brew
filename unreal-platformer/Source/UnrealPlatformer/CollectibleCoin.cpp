#include "CollectibleCoin.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "UObject/ConstructorHelpers.h"

ACollectibleCoin::ACollectibleCoin()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere.Get());
	CollisionSphere->InitSphereRadius(60.f);
	CollisionSphere->SetMobility(EComponentMobility::Movable);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetGenerateOverlapEvents(true);

	// A squashed cylinder standing on its edge reads as a coin.
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(CollisionSphere.Get());
	CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CoinMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	CoinMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.1f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		CoinMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		CoinMesh->SetMaterial(0, BaseMaterial.Object);
	}

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 180.f, 0.f);
}

void ACollectibleCoin::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* Material = CoinMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.85f, 0.1f));
	}

	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->RegisterCoin();
	}
}

void ACollectibleCoin::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Cast<APlatformerCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->AddCoin();
	}

	Destroy();
}
