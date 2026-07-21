#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneTrigger.generated.h"

class UBoxComponent;

/** Invisible volume that shows a zone banner when the player wanders in. */
UCLASS()
class MEMORYQUEST_API AZoneTrigger : public AActor
{
	GENERATED_BODY()

public:
	AZoneTrigger();

	/** Set before FinishSpawning. */
	void SetZoneText(const FString& InName, const FString& InSubtitle)
	{
		ZoneName = InName;
		ZoneSubtitle = InSubtitle;
	}

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Zone")
	TObjectPtr<UBoxComponent> Bounds;

	FString ZoneName;
	FString ZoneSubtitle;
};
