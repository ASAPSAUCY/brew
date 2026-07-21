#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectibleCoin.generated.h"

class URotatingMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

/** A spinning gold coin. Overlapping it with the player adds one to the score. */
UCLASS()
class UNREALPLATFORMER_API ACollectibleCoin : public AActor
{
	GENERATED_BODY()

public:
	ACollectibleCoin();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Coin")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Coin")
	TObjectPtr<UStaticMeshComponent> CoinMesh;

	UPROPERTY(VisibleAnywhere, Category = "Coin")
	TObjectPtr<URotatingMovementComponent> RotatingMovement;
};
