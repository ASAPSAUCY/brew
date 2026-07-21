#pragma once

#include "CoreMinimal.h"
#include "MQInteractable.h"
#include "EscapeRoomActors.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/**
 * The locked door of The Final Room - the one escape room Kyle and Claudia
 * never finished (the mimosas won). Opens once enough memories and keepsakes
 * have been gathered.
 */
UCLASS()
class MEMORYQUEST_API AEscapeDoor : public AMQInteractable
{
	GENERATED_BODY()

public:
	AEscapeDoor();

	virtual FString GetPrompt() const override;
	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
};

/**
 * One of three pedestals inside The Final Room. They must be pressed in the
 * order the story happened: the party, the courts, New York.
 */
UCLASS()
class MEMORYQUEST_API APuzzlePedestal : public AMQInteractable
{
	GENERATED_BODY()

public:
	APuzzlePedestal();

	/** Set before FinishSpawning. Index is the correct press order (0-based). */
	void SetPuzzleIndex(int32 InIndex) { PuzzleIndex = InIndex; }
	int32 GetPuzzleIndex() const { return PuzzleIndex; }

	virtual void Interact(AMQCharacter* Player) override;

	void MarkSolved();
	void ResetPedestal();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Puzzle")
	TObjectPtr<UStaticMeshComponent> ColumnMesh;

	UPROPERTY(VisibleAnywhere, Category = "Puzzle")
	TObjectPtr<UStaticMeshComponent> OrbMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OrbMaterial;

	FLinearColor OrbColor = FLinearColor::White;
	int32 PuzzleIndex = 0;
};

/** The golden trophy that ends the game. Four years in the making. */
UCLASS()
class MEMORYQUEST_API ALevelTrophy : public AMQInteractable
{
	GENERATED_BODY()

public:
	ALevelTrophy();

	virtual void Interact(AMQCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Trophy")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Trophy")
	TObjectPtr<UStaticMeshComponent> CupMesh;
};
