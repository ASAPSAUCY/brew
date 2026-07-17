#include "MQCharacter.h"

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
#include "MQGameMode.h"
#include "MQInteractable.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StoryData.h"
#include "UObject/ConstructorHelpers.h"

AMQCharacter::AMQCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 540.f, 0.f);
	Movement->JumpZVelocity = 550.f;
	Movement->AirControl = 0.4f;
	Movement->GravityScale = 1.3f;
	Movement->MaxWalkSpeed = 600.f;
	Movement->MinAnalogWalkSpeed = 20.f;
	Movement->BrakingDecelerationWalking = 2000.f;

	// Fixed cozy-camera: high, pitched down, never rotates with the character.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1500.f;
	CameraBoom->SetRelativeRotation(FRotator(-52.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -20.f));
	BodyMesh->SetRelativeScale3D(FVector(0.62f, 0.62f, 1.3f));

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetCapsuleComponent());
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetRelativeLocation(FVector(0.f, 0.f, 68.f));
	HeadMesh->SetRelativeScale3D(FVector(0.52f, 0.52f, 0.52f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		HeadMesh->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial.Succeeded())
	{
		BodyMesh->SetMaterial(0, BaseMaterial.Object);
		HeadMesh->SetMaterial(0, BaseMaterial.Object);
	}
}

void AMQCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* BodyMaterial = BodyMesh->CreateDynamicMaterialInstance(0))
	{
		BodyMaterial->SetVectorParameterValue(TEXT("Color"), StoryData::PlayerColor());
	}
	if (UMaterialInstanceDynamic* HeadMaterial = HeadMesh->CreateDynamicMaterialInstance(0))
	{
		HeadMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.92f, 0.76f, 0.60f));
	}
}

void AMQCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMQCharacter::Move);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AMQCharacter::DoJumpStart);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AMQCharacter::OnInteract);
		EnhancedInput->BindAction(JournalAction, ETriggerEvent::Started, this, &AMQCharacter::OnJournal);
		EnhancedInput->BindAction(Choice1Action, ETriggerEvent::Started, this, &AMQCharacter::OnChoice1);
		EnhancedInput->BindAction(Choice2Action, ETriggerEvent::Started, this, &AMQCharacter::OnChoice2);
		EnhancedInput->BindAction(Choice3Action, ETriggerEvent::Started, this, &AMQCharacter::OnChoice3);
		EnhancedInput->BindAction(Choice4Action, ETriggerEvent::Started, this, &AMQCharacter::OnChoice4);
	}
}

void AMQCharacter::InitializeInputObjects()
{
	MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis2D;

	JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
	InteractAction = NewObject<UInputAction>(this, TEXT("IA_Interact"));
	JournalAction = NewObject<UInputAction>(this, TEXT("IA_Journal"));
	Choice1Action = NewObject<UInputAction>(this, TEXT("IA_Choice1"));
	Choice2Action = NewObject<UInputAction>(this, TEXT("IA_Choice2"));
	Choice3Action = NewObject<UInputAction>(this, TEXT("IA_Choice3"));
	Choice4Action = NewObject<UInputAction>(this, TEXT("IA_Choice4"));

	DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_MemoryQuest"));

	// Move is Axis2D: X = screen-right, Y = screen-up. Key presses arrive in X,
	// so SwizzleAxis (default YXZ) reroutes W/S into Y and Negate flips reverses.
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

	DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);
	DefaultMappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);

	DefaultMappingContext->MapKey(InteractAction, EKeys::E);
	DefaultMappingContext->MapKey(InteractAction, EKeys::Gamepad_FaceButton_Left);

	DefaultMappingContext->MapKey(JournalAction, EKeys::Tab);
	DefaultMappingContext->MapKey(JournalAction, EKeys::Gamepad_FaceButton_Top);

	DefaultMappingContext->MapKey(Choice1Action, EKeys::One);
	DefaultMappingContext->MapKey(Choice1Action, EKeys::Gamepad_DPad_Up);
	DefaultMappingContext->MapKey(Choice2Action, EKeys::Two);
	DefaultMappingContext->MapKey(Choice2Action, EKeys::Gamepad_DPad_Right);
	DefaultMappingContext->MapKey(Choice3Action, EKeys::Three);
	DefaultMappingContext->MapKey(Choice3Action, EKeys::Gamepad_DPad_Down);
	DefaultMappingContext->MapKey(Choice4Action, EKeys::Four);
	DefaultMappingContext->MapKey(Choice4Action, EKeys::Gamepad_DPad_Left);
}

void AMQCharacter::Move(const FInputActionValue& Value)
{
	if (bFrozen || Controller == nullptr)
	{
		return;
	}

	// The camera never rotates, so movement is in fixed world axes:
	// screen-up is +X, screen-right is +Y.
	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(FVector::ForwardVector, MovementVector.Y);
	AddMovementInput(FVector::RightVector, MovementVector.X);
}

void AMQCharacter::DoJumpStart()
{
	if (!bFrozen)
	{
		Jump();
	}
}

void AMQCharacter::OnInteract()
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->OnInteractPressed(this);
	}
}

void AMQCharacter::OnJournal()
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->OnJournalPressed();
	}
}

void AMQCharacter::OnChoice1() { SendChoice(1); }
void AMQCharacter::OnChoice2() { SendChoice(2); }
void AMQCharacter::OnChoice3() { SendChoice(3); }
void AMQCharacter::OnChoice4() { SendChoice(4); }

void AMQCharacter::SendChoice(int32 ChoiceNumber)
{
	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->OnChoicePressed(ChoiceNumber);
	}
}

void AMQCharacter::SetFrozen(bool bInFrozen)
{
	bFrozen = bInFrozen;

	if (bFrozen)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
