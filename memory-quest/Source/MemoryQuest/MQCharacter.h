#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MQCharacter.generated.h"

class AMQInteractable;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UStaticMeshComponent;
struct FInputActionValue;

/**
 * Kyle. Fixed Animal Crossing-style camera (no mouse needed), WASD movement,
 * E to interact, Tab for the scrapbook, 1-4 for battle choices. All Enhanced
 * Input objects are built in code - the project ships no input assets.
 */
UCLASS()
class MEMORYQUEST_API AMQCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMQCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Freezes/unfreezes movement while dialogue, battles or menus are up. */
	void SetFrozen(bool bInFrozen);

	void SetFocusedInteractable(AMQInteractable* Interactable) { FocusedInteractable = Interactable; }
	void ClearFocusedInteractable(AMQInteractable* Interactable)
	{
		if (FocusedInteractable.Get() == Interactable)
		{
			FocusedInteractable = nullptr;
		}
	}
	AMQInteractable* GetFocusedInteractable() const { return FocusedInteractable.Get(); }

protected:
	virtual void BeginPlay() override;

private:
	void InitializeInputObjects();

	void Move(const FInputActionValue& Value);
	void DoJumpStart();
	void OnInteract();
	void OnJournal();
	void OnChoice1();
	void OnChoice2();
	void OnChoice3();
	void OnChoice4();
	void SendChoice(int32 ChoiceNumber);

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Visuals")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visuals")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	// Runtime-built input assets, kept in UPROPERTYs so the GC leaves them alone.
	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY()
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY()
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY()
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY()
	TObjectPtr<UInputAction> JournalAction;

	UPROPERTY()
	TObjectPtr<UInputAction> Choice1Action;

	UPROPERTY()
	TObjectPtr<UInputAction> Choice2Action;

	UPROPERTY()
	TObjectPtr<UInputAction> Choice3Action;

	UPROPERTY()
	TObjectPtr<UInputAction> Choice4Action;

	TWeakObjectPtr<AMQInteractable> FocusedInteractable;

	bool bFrozen = false;
};
