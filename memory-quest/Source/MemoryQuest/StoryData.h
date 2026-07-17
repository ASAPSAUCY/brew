#pragma once

#include "CoreMinimal.h"

// ============================================================================
//  THE STORY FILE
//  Every personal detail in the game lives here: names, zones, creatures,
//  keepsakes, dialogue. Edit this file (and StoryData.cpp) to change what the
//  world says - no other code needs to be touched.
// ============================================================================

/** Which basic engine shape a creature's body uses. */
enum class EMQShape : uint8
{
	Sphere,
	Cube,
	Cylinder
};

/** A catchable memory creature ("Memorydex" entry). */
struct FMQCreatureDef
{
	FString Name;          // Display name, e.g. "Mimossy"
	FString Species;       // One-line species flavor, e.g. "Brunch Spirit"
	FString Zone;          // Zone label shown on the memory card
	FString MemoryTitle;   // Memory card headline
	FString MemoryText;    // The actual memory - the emotional payload
	FString AttackA;       // Playful battle move names
	FString AttackB;
	EMQShape Shape = EMQShape::Sphere;
	float Size = 1.f;      // Body scale multiplier
	FLinearColor BodyColor = FLinearColor::White;
	FLinearColor TopColor = FLinearColor::White;
	int32 MaxHP = 70;
	FVector SpawnLocation = FVector::ZeroVector;
	float WanderRadius = 400.f;
	bool bSpecial = false; // Special entries (Rumor) are remembered, not battled
};

/** A collectible keepsake for the scrapbook. */
struct FMQKeepsakeDef
{
	FString Name;
	FString MemoryTitle;
	FString MemoryText;
	FLinearColor Color = FLinearColor::White;
	FVector SpawnLocation = FVector::ZeroVector;
};

/** A talkable NPC and their dialogue pages. */
struct FMQNpcDef
{
	FString Name;
	FLinearColor Color = FLinearColor::White;
	FVector SpawnLocation = FVector::ZeroVector;
	float FacingYaw = 180.f;
	TArray<FString> Pages;
};

namespace StoryData
{
	// ---- The couple -------------------------------------------------------
	FString PlayerName();          // "Kyle"
	FString PartnerName();         // "Claudia"
	FLinearColor PlayerColor();    // Tiffany blue
	FLinearColor PartnerColor();   // Red

	// ---- Title / banner text ----------------------------------------------
	FString GameTitle();
	FString GameSubtitle();

	// ---- World content -----------------------------------------------------
	const TArray<FMQCreatureDef>& Creatures();
	const TArray<FMQKeepsakeDef>& Keepsakes();
	const TArray<FMQNpcDef>& Npcs();

	// ---- The dogs ------------------------------------------------------------
	FString BuddyName();
	const TArray<FString>& BuddyPages();

	// ---- Quest tuning ------------------------------------------------------
	int32 CreaturesNeededForEscapeRoom(); // 8
	int32 KeepsakesNeededForEscapeRoom(); // 6

	// ---- Escape room -------------------------------------------------------
	// Pedestal clue text, in the order they must be pressed (the order the
	// story happened: the party, the courts, New York).
	const TArray<FString>& PuzzleClues();
	FString PuzzleWrongOrderText();
	FString PuzzleSolvedText();

	// ---- Finale ------------------------------------------------------------
	const TArray<FString>& FinaleLines();
}
