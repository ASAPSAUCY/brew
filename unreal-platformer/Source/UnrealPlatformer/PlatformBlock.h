#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformBlock.generated.h"

class UStaticMeshComponent;

/**
 * A colored box of solid geometry. All static level geometry is spawned from
 * this class (or a subclass); its size comes from the spawn transform's scale
 * (the engine cube is 100 units, so a scale of (6, 6, 0.5) is a 600x600x50 slab).
 */
UCLASS()
class UNREALPLATFORMER_API APlatformBlock : public AActor
{
	GENERATED_BODY()

public:
	APlatformBlock();

	/** Only meaningful before BeginPlay (i.e. between deferred spawn and FinishSpawning). */
	void SetBaseColor(const FLinearColor& Color) { BaseColor = Color; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Platform")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Platform")
	FLinearColor BaseColor = FLinearColor(0.5f, 0.5f, 0.5f);
};
