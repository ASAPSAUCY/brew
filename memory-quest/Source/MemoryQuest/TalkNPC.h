#pragma once

#include "CoreMinimal.h"
#include "MQInteractable.h"
#include "TalkNPC.generated.h"

class UStaticMeshComponent;

/** A villager who tells part of the story. Content comes from StoryData::Npcs(). */
UCLASS()
class MEMORYQUEST_API ATalkNPC : public AMQInteractable
{
	GENERATED_BODY()

public:
	ATalkNPC();

	/** Which StoryData::Npcs() entry this villager is. Set before FinishSpawning. */
	void SetNpcIndex(int32 InIndex) { NpcIndex = InIndex; }

	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "NPC")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "NPC")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, Category = "NPC")
	TObjectPtr<UStaticMeshComponent> LeftEye;

	UPROPERTY(VisibleAnywhere, Category = "NPC")
	TObjectPtr<UStaticMeshComponent> RightEye;

	UPROPERTY(VisibleAnywhere, Category = "NPC")
	TObjectPtr<UStaticMeshComponent> LeftPupil;

	UPROPERTY(VisibleAnywhere, Category = "NPC")
	TObjectPtr<UStaticMeshComponent> RightPupil;

	int32 NpcIndex = 0;
};
