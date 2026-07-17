#include "ZoneTrigger.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "MQCharacter.h"
#include "MQGameMode.h"

AZoneTrigger::AZoneTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Extent 50 so the spawn transform's scale maps 1:1 with a 100-unit box.
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	SetRootComponent(Bounds);
	Bounds->InitBoxExtent(FVector(50.f, 50.f, 50.f));
	Bounds->SetMobility(EComponentMobility::Movable);
	Bounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Bounds->SetCollisionObjectType(ECC_WorldDynamic);
	Bounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	Bounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Bounds->SetGenerateOverlapEvents(true);
}

void AZoneTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Cast<AMQCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	if (AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>())
	{
		GameMode->AnnounceZone(ZoneName, ZoneSubtitle);
	}
}
