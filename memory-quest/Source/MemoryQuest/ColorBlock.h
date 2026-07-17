#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoryData.h"
#include "ColorBlock.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

/**
 * A tinted primitive (cube, sphere or cylinder) - every piece of scenery in the
 * world is one of these. Size comes from the spawn transform's scale (the
 * engine shapes are 100 units at scale 1). Shape and color must be set between
 * deferred spawn and FinishSpawning.
 */
UCLASS()
class MEMORYQUEST_API AColorBlock : public AActor
{
	GENERATED_BODY()

public:
	AColorBlock();

	void SetBaseColor(const FLinearColor& Color) { BaseColor = Color; }
	void SetShape(EMQShape InShape) { Shape = InShape; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Block")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereAsset;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderAsset;

	UPROPERTY(EditAnywhere, Category = "Block")
	FLinearColor BaseColor = FLinearColor(0.5f, 0.5f, 0.5f);

	EMQShape Shape = EMQShape::Cube;
};
