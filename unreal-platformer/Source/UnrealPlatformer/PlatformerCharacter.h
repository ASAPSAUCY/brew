#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlatformerCharacter.generated.h"

class UCameraComponent;
class UDamageType;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UStaticMeshComponent;
struct FInputActionValue;

/**
 * Third-person platforming character: spring-arm camera, double jump, and
 * Enhanced Input actions/mappings that are constructed entirely in code
 * (the project ships no input data assets).
 */
UCLASS()
class UNREALPLATFORMER_API APlatformerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlatformerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Falling past KillZ counts as a death; the game mode decides what happens next. */
	virtual void FellOutOfWorld(const UDamageType& DmgType) override;

	/** Teleports the character back to a checkpoint and clears its momentum. */
	void MoveToRespawnPoint(const FVector& Location, const FRotator& Rotation);

	/** Stops movement once the game is over. Restart (R) stays active. */
	void DisableGameplayInput();

protected:
	virtual void BeginPlay() override;

private:
	/** Builds the input actions and mapping context that would normally be .uassets. */
	void InitializeInputObjects();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void RequestRestart();

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Visuals")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	// The runtime-built input assets live in UPROPERTYs so the GC keeps them alive.
	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY()
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY()
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY()
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY()
	TObjectPtr<UInputAction> RestartAction;

	/** Flip if moving the mouse up should look down instead of up. */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bInvertLookY = false;

	bool bGameplayInputEnabled = true;
};
