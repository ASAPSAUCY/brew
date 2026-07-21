#include "ColorBlock.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AColorBlock::AColorBlock()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent.Get());
	MeshComponent->SetMobility(EComponentMobility::Movable);

	// The cube is the default; the alternatives are cached so BeginPlay can
	// swap them in (asset loading is only legal in constructors).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		SphereAsset = SphereMesh.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		CylinderAsset = CylinderMesh.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		MeshComponent->SetMaterial(0, BaseMaterial.Object);
	}
}

void AColorBlock::BeginPlay()
{
	Super::BeginPlay();

	if (Shape == EMQShape::Sphere && SphereAsset != nullptr)
	{
		MeshComponent->SetStaticMesh(SphereAsset);
	}
	else if (Shape == EMQShape::Cylinder && CylinderAsset != nullptr)
	{
		MeshComponent->SetStaticMesh(CylinderAsset);
	}

	if (UMaterialInstanceDynamic* Material = MeshComponent->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), BaseColor);
	}
}
