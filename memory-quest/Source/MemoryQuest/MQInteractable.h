#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MQInteractable.generated.h"

class AMQCharacter;
class USphereComponent;
class UTextRenderComponent;

/**
 * Base class for everything the player can walk up to and press E on: NPCs,
 * coins... er, keepsakes, creatures, doors, pedestals. Walking into the
 * interaction radius focuses this actor on the player character; the HUD shows
 * GetPrompt() and E routes to Interact().
 */
UCLASS()
class MEMORYQUEST_API AMQInteractable : public AActor
{
	GENERATED_BODY()

public:
	AMQInteractable();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	/** What happens on E. Subclasses override. */
	virtual void Interact(AMQCharacter* Player);

	virtual FString GetPrompt() const { return PromptText; }
	void SetPrompt(const FString& InPrompt) { PromptText = InPrompt; }

	/** Sets the floating name label above the actor. */
	void SetLabel(const FString& InLabel);

protected:
	/** Keeps the label facing the fixed camera even if the actor rotates. */
	void FaceLabelToCamera();

	UPROPERTY(VisibleAnywhere, Category = "Interactable")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Interactable")
	TObjectPtr<UTextRenderComponent> NameLabel;

	FString PromptText;
};
