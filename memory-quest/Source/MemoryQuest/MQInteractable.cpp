#include "MQInteractable.h"

#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "MQCharacter.h"

AMQInteractable::AMQInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	SetRootComponent(InteractionSphere);
	InteractionSphere->InitSphereRadius(220.f);
	InteractionSphere->SetMobility(EComponentMobility::Movable);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->SetGenerateOverlapEvents(true);

	NameLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameLabel"));
	NameLabel->SetupAttachment(InteractionSphere);
	NameLabel->SetRelativeLocation(FVector(0.f, 0.f, 175.f));
	// The camera looks along +X, so labels face back down -X.
	NameLabel->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	NameLabel->SetHorizontalAlignment(EHTA_Center);
	NameLabel->SetWorldSize(44.f);
	NameLabel->SetTextRenderColor(FColor::White);
	NameLabel->SetText(FText::GetEmpty());
}

void AMQInteractable::SetLabel(const FString& InLabel)
{
	NameLabel->SetText(FText::FromString(InLabel));
}

void AMQInteractable::FaceLabelToCamera()
{
	NameLabel->SetWorldRotation(FRotator(0.f, 180.f, 0.f));
}

void AMQInteractable::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (AMQCharacter* Player = Cast<AMQCharacter>(OtherActor))
	{
		Player->SetFocusedInteractable(this);
	}
}

void AMQInteractable::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (AMQCharacter* Player = Cast<AMQCharacter>(OtherActor))
	{
		Player->ClearFocusedInteractable(this);
	}
}

void AMQInteractable::Interact(AMQCharacter* Player)
{
	// Base interactables do nothing.
}
