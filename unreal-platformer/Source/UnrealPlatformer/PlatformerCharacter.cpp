#include "PlatformerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputTriggers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "PlatformerGameMode.h"
#include "UObject/ConstructorHelpers.h"

APlatformerCharacter::APlatformerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// The controller only steers the camera; the pawn turns to face its motion.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	JumpMaxCount = 2;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 500.f, 0.f);
	Movement->JumpZVelocity = 700.f;
	Movement->AirControl = 0.35f;
	Movement->GravityScale = 1.5f;
	Movement->MaxWalkSpeed = 600.f;
	Movement->MinAnalogWalkSpeed = 20.f;
	Movement->BrakingDecelerationWalking = 2000.f;
	Movement->BrakingDecelerationFalling = 150.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom.Get(), USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// No skeletal mesh assets exist in this project, so the body is a scaled cube.
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.8f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		BodyMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void APlatformerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* Material = BodyMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.15f, 0.35f, 0.95f));
	}
}

void APlatformerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (DefaultMappingContext == nullptr)
	{
		InitializeInputObjects();
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformerCharacter::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlatformerCharacter::Look);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInput->BindAction(RestartAction, ETriggerEvent::Started, this, &APlatformerCharacter::RequestRestart);
	}
}

void APlatformerCharacter::InitializeInputObjects()
{
	MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis2D;

	LookAction = NewObject<UInputAction>(this, TEXT("IA_Look"));
	LookAction->ValueType = EInputActionValueType::Axis2D;

	JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
	RestartAction = NewObject<UInputAction>(this, TEXT("IA_Restart"));

	DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Platformer"));

	// Move is an Axis2D action: X = right, Y = forward. A key press arrives in the
	// X component, so SwizzleAxis (default order YXZ) reroutes it into Y for the
	// forward/backward keys, and Negate flips the reverse directions.
	FEnhancedActionKeyMapping& ForwardKey = DefaultMappingContext->MapKey(MoveAction, EKeys::W);
	ForwardKey.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(this));

	FEnhancedActionKeyMapping& BackwardKey = DefaultMappingContext->MapKey(MoveAction, EKeys::S);
	BackwardKey.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(this));
	BackwardKey.Modifiers.Add(NewObject<UInputModifierNegate>(this));

	DefaultMappingContext->MapKey(MoveAction, EKeys::D);

	FEnhancedActionKeyMapping& LeftKey = DefaultMappingContext->MapKey(MoveAction, EKeys::A);
	LeftKey.Modifiers.Add(NewObject<UInputModifierNegate>(this));

	FEnhancedActionKeyMapping& GamepadMoveY = DefaultMappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftY);
	GamepadMoveY.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(this));
	DefaultMappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftX);

	// Look: the mouse delivers both axes at once through the paired Mouse2D key.
	DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);
	DefaultMappingContext->MapKey(LookAction, EKeys::Gamepad_RightX);
	FEnhancedActionKeyMapping& GamepadLookY = DefaultMappingContext->MapKey(LookAction, EKeys::Gamepad_RightY);
	GamepadLookY.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(this));

	DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);
	DefaultMappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);

	DefaultMappingContext->MapKey(RestartAction, EKeys::R);
	DefaultMappingContext->MapKey(RestartAction, EKeys::Gamepad_Special_Right);
}

void APlatformerCharacter::Move(const FInputActionValue& Value)
{
	if (!bGameplayInputEnabled || Controller == nullptr)
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlatformerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(bInvertLookY ? -LookVector.Y : LookVector.Y);
}

void APlatformerCharacter::RequestRestart()
{
	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->RestartLevel();
	}
}

void APlatformerCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
	// Deliberately not calling Super: the default implementation destroys the pawn,
	// but here a fall is an ordinary death that ends in a checkpoint respawn.
	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->HandlePlayerDeath(this);
	}
}

void APlatformerCharacter::MoveToRespawnPoint(const FVector& Location, const FRotator& Rotation)
{
	GetCharacterMovement()->StopMovementImmediately();
	SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(FRotator(0.f, Rotation.Yaw, 0.f));

	if (Controller != nullptr)
	{
		Controller->SetControlRotation(Rotation);
	}
}

void APlatformerCharacter::DisableGameplayInput()
{
	bGameplayInputEnabled = false;
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}
