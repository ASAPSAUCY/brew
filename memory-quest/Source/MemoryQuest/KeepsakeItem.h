#pragma once

#include "CoreMinimal.h"
#include "MQInteractable.h"
#include "KeepsakeItem.generated.h"

class URotatingMovementComponent;
class UStaticMeshComponent;

/** A slowly spinning keepsake for the scrapbook. Content: StoryData::Keepsakes(). */
UCLASS()
class MEMORYQUEST_API AKeepsakeItem : public AMQInteractable
{
	GENERATED_BODY()

public:
	AKeepsakeItem();

	/** Which StoryData::Keepsakes() entry this is. Set before FinishSpawning. */
	void SetKeepsakeIndex(int32 InIndex) { KeepsakeIndex = InIndex; }

	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Keepsake")
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Keepsake")
	TObjectPtr<URotatingMovementComponent> RotatingMovement;

	int32 KeepsakeIndex = 0;
};
