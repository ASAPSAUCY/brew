#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StoryData.h"
#include "MQGameMode.generated.h"

class AMemoryCreature;
class AMQCharacter;
class APuzzlePedestal;

/** What the player is currently doing; drives input routing and the HUD. */
UENUM()
enum class EMQFlow : uint8
{
	Explore,
	Dialogue,
	Battle,
	MemoryCard,
	Scrapbook,
	Finale
};

/** Live state of the current (gentle) creature battle. */
struct FMQBattleState
{
	int32 CreatureIndex = -1;
	int32 HP = 0;
	int32 MaxHP = 1;
	int32 Charm = 0;
	int32 Heart = 100;
	TArray<FString> Log;
};

/** A full-screen memory card shown when something is caught or collected. */
struct FMQMemoryCard
{
	FString Title;
	FString Subtitle;
	FString Text;
};

/**
 * Runs the whole game: world bootstrap, the explore/dialogue/battle/scrapbook
 * flow, the Memorydex and keepsake collections, the escape-room quest and the
 * finale. The HUD renders whatever state lives here.
 */
UCLASS()
class MEMORYQUEST_API AMQGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMQGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	// ---- Input routing (called by the character) ---------------------------
	void OnInteractPressed(AMQCharacter* Player);
	void OnJournalPressed();
	void OnChoicePressed(int32 ChoiceNumber);

	// ---- Dialogue -----------------------------------------------------------
	void StartDialogue(const FString& SpeakerName, const TArray<FString>& Pages, const FLinearColor& SpeakerColor);

	// ---- Battle -------------------------------------------------------------
	void StartBattle(AMemoryCreature* Creature);

	// ---- Collections --------------------------------------------------------
	void CollectKeepsake(int32 KeepsakeIndex);
	void RememberRumor();

	// ---- Zones / banner -----------------------------------------------------
	void AnnounceZone(const FString& ZoneName, const FString& Subtitle);

	// ---- Escape room ---------------------------------------------------------
	bool IsEscapeRoomUnlocked() const;
	void OnEscapeDoorOpened();
	void RegisterPedestal(APuzzlePedestal* Pedestal);
	void ActivatePedestal(APuzzlePedestal* Pedestal);
	void SetTrophySpawnPoint(const FVector& Location) { TrophySpawnPoint = Location; }
	void StartFinale();

	// ---- HUD getters ----------------------------------------------------------
	EMQFlow GetFlow() const { return Flow; }
	int32 GetCaughtCount() const;
	int32 GetKeepsakeCount() const;
	int32 GetTotalCreatures() const;
	int32 GetTotalKeepsakes() const;
	bool IsCreatureCaught(int32 Index) const { return CaughtFlags.IsValidIndex(Index) && CaughtFlags[Index]; }
	bool IsKeepsakeCollected(int32 Index) const { return KeepsakeFlags.IsValidIndex(Index) && KeepsakeFlags[Index]; }

	const FString& GetDialogueSpeaker() const { return DialogueSpeaker; }
	const FLinearColor& GetDialogueColor() const { return DialogueColor; }
	FString GetDialoguePage() const;
	int32 GetDialoguePageNumber() const { return DialoguePageIndex + 1; }
	int32 GetDialoguePageCount() const { return DialoguePages.Num(); }

	const FMQBattleState& GetBattle() const { return Battle; }
	const FMQMemoryCard& GetMemoryCard() const { return MemoryCard; }

	const FString& GetBannerTitle() const { return BannerTitle; }
	const FString& GetBannerSubtitle() const { return BannerSubtitle; }
	float GetBannerHideTime() const { return BannerHideTime; }

private:
	void SetPlayerFrozen(bool bFrozen);
	void ShowMemoryCard(const FString& Title, const FString& Subtitle, const FString& Text);
	void AddBattleLog(const FString& Line);
	void CreatureTurn();
	void EndBattle(const FString& PartingLine);
	void MarkCreatureCaught(int32 Index);
	void ShowBanner(const FString& Title, const FString& Subtitle, float Duration);

	EMQFlow Flow = EMQFlow::Explore;

	// Collections
	TArray<bool> CaughtFlags;
	TArray<bool> KeepsakeFlags;

	// Dialogue
	FString DialogueSpeaker;
	FLinearColor DialogueColor = FLinearColor::White;
	TArray<FString> DialoguePages;
	int32 DialoguePageIndex = 0;

	// Battle
	FMQBattleState Battle;
	TWeakObjectPtr<AMemoryCreature> BattleCreature;

	// Memory card
	FMQMemoryCard MemoryCard;

	// Zone banner
	FString CurrentZoneName;
	FString BannerTitle;
	FString BannerSubtitle;
	float BannerHideTime = 0.f;

	// Escape room
	UPROPERTY()
	TArray<TObjectPtr<APuzzlePedestal>> Pedestals;
	int32 NextPedestalIndex = 0;
	bool bPuzzleSolved = false;
	FVector TrophySpawnPoint = FVector::ZeroVector;
};
