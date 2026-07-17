#include "MQGameMode.h"

#include "Engine/World.h"
#include "EscapeRoomActors.h"
#include "Kismet/GameplayStatics.h"
#include "MQCharacter.h"
#include "MQHUD.h"
#include "MQInteractable.h"
#include "MemoryCreature.h"
#include "WorldBuilder.h"

AMQGameMode::AMQGameMode()
{
	DefaultPawnClass = AMQCharacter::StaticClass();
	HUDClass = AMQHUD::StaticClass();
}

void AMQGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	CaughtFlags.Init(false, StoryData::Creatures().Num());
	KeepsakeFlags.Init(false, StoryData::Keepsakes().Num());

	// Build the world now: InitGame runs before the player logs in, so the
	// PlayerStart the builder spawns exists by the time RestartPlayer needs it.
	if (AWorldBuilder* Builder = GetWorld()->SpawnActor<AWorldBuilder>())
	{
		Builder->BuildWorld();
	}
}

void AMQGameMode::StartPlay()
{
	Super::StartPlay();

	ShowBanner(StoryData::GameTitle(), StoryData::GameSubtitle(), 6.f);
}

// ---------------------------------------------------------------------------
// Input routing
// ---------------------------------------------------------------------------

void AMQGameMode::OnInteractPressed(AMQCharacter* Player)
{
	switch (Flow)
	{
	case EMQFlow::Explore:
		if (Player != nullptr)
		{
			if (AMQInteractable* Focused = Player->GetFocusedInteractable())
			{
				Focused->Interact(Player);
			}
		}
		break;

	case EMQFlow::Dialogue:
		++DialoguePageIndex;
		if (DialoguePageIndex >= DialoguePages.Num())
		{
			Flow = EMQFlow::Explore;
			SetPlayerFrozen(false);
		}
		break;

	case EMQFlow::MemoryCard:
	case EMQFlow::Finale:
		Flow = EMQFlow::Explore;
		SetPlayerFrozen(false);
		break;

	default:
		break;
	}
}

void AMQGameMode::OnJournalPressed()
{
	if (Flow == EMQFlow::Explore)
	{
		Flow = EMQFlow::Scrapbook;
		SetPlayerFrozen(true);
	}
	else if (Flow == EMQFlow::Scrapbook)
	{
		Flow = EMQFlow::Explore;
		SetPlayerFrozen(false);
	}
}

void AMQGameMode::OnChoicePressed(int32 ChoiceNumber)
{
	if (Flow != EMQFlow::Battle || Battle.CreatureIndex == INDEX_NONE)
	{
		return;
	}

	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	if (!Creatures.IsValidIndex(Battle.CreatureIndex))
	{
		return;
	}
	const FMQCreatureDef& Def = Creatures[Battle.CreatureIndex];

	switch (ChoiceNumber)
	{
	case 1: // Tease: wears the memory down, but never below 1 - this is a cozy game.
	{
		const int32 Damage = FMath::RandRange(20, 36);
		Battle.HP = FMath::Max(1, Battle.HP - Damage);
		AddBattleLog(FString::Printf(TEXT("You teased %s! Its resolve drops by %d."), *Def.Name, Damage));
		if (Battle.HP == 1)
		{
			AddBattleLog(FString::Printf(TEXT("%s is all tired out! Now's your moment!"), *Def.Name));
		}
		CreatureTurn();
		break;
	}
	case 2: // Sweet talk: raises catch chance.
	{
		Battle.Charm = FMath::Min(Battle.Charm + 1, 3);
		AddBattleLog(FString::Printf(TEXT("You sweet-talked %s. It's blushing! (Charm up)"), *Def.Name));
		CreatureTurn();
		break;
	}
	case 3: // Give a treat: the catch attempt.
	{
		float Chance = 0.18f + 0.55f * (1.f - static_cast<float>(Battle.HP) / static_cast<float>(Battle.MaxHP))
			+ 0.12f * static_cast<float>(Battle.Charm);
		Chance = FMath::Min(Chance, 0.95f);

		if (FMath::FRand() < Chance)
		{
			AddBattleLog(FString::Printf(TEXT("%s takes the treat... and decides it likes you!"), *Def.Name));
			MarkCreatureCaught(Battle.CreatureIndex);

			if (AMemoryCreature* Creature = BattleCreature.Get())
			{
				Creature->Destroy();
			}
			BattleCreature = nullptr;

			ShowMemoryCard(Def.MemoryTitle,
				FString::Printf(TEXT("%s - %s - %s"), *Def.Name, *Def.Species, *Def.Zone),
				Def.MemoryText);
			Flow = EMQFlow::MemoryCard; // Player stays frozen for the card.
		}
		else
		{
			AddBattleLog(FString::Printf(TEXT("%s wriggles free of the treat! So dramatic."), *Def.Name));
			CreatureTurn();
		}
		break;
	}
	case 4:
		EndBattle(FString::Printf(TEXT("You wave goodbye. %s waves back. Probably."), *Def.Name));
		break;

	default:
		break;
	}
}

// ---------------------------------------------------------------------------
// Dialogue
// ---------------------------------------------------------------------------

void AMQGameMode::StartDialogue(const FString& SpeakerName, const TArray<FString>& Pages, const FLinearColor& SpeakerColor)
{
	if (Flow != EMQFlow::Explore || Pages.Num() == 0)
	{
		return;
	}

	Flow = EMQFlow::Dialogue;
	DialogueSpeaker = SpeakerName;
	DialogueColor = SpeakerColor;
	DialoguePages = Pages;
	DialoguePageIndex = 0;
	SetPlayerFrozen(true);
}

FString AMQGameMode::GetDialoguePage() const
{
	return DialoguePages.IsValidIndex(DialoguePageIndex) ? DialoguePages[DialoguePageIndex] : FString();
}

// ---------------------------------------------------------------------------
// Battle
// ---------------------------------------------------------------------------

void AMQGameMode::StartBattle(AMemoryCreature* Creature)
{
	if (Flow != EMQFlow::Explore || Creature == nullptr)
	{
		return;
	}

	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	const int32 Index = Creature->GetCreatureIndex();
	if (!Creatures.IsValidIndex(Index) || Creatures[Index].bSpecial)
	{
		return;
	}
	const FMQCreatureDef& Def = Creatures[Index];

	Flow = EMQFlow::Battle;
	SetPlayerFrozen(true);
	Creature->SetInBattle(true);
	BattleCreature = Creature;

	Battle = FMQBattleState();
	Battle.CreatureIndex = Index;
	Battle.MaxHP = Def.MaxHP;
	Battle.HP = Def.MaxHP;
	AddBattleLog(FString::Printf(TEXT("A wild memory appeared: %s, the %s!"), *Def.Name, *Def.Species));
	AddBattleLog(TEXT("Wear it down, sweet-talk it, then win it over with a treat."));
}

void AMQGameMode::CreatureTurn()
{
	if (Flow != EMQFlow::Battle)
	{
		return;
	}

	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	if (!Creatures.IsValidIndex(Battle.CreatureIndex))
	{
		return;
	}
	const FMQCreatureDef& Def = Creatures[Battle.CreatureIndex];

	const FString AttackName = FMath::RandBool() ? Def.AttackA : Def.AttackB;
	const int32 Damage = FMath::RandRange(8, 16);
	Battle.Heart -= Damage;
	AddBattleLog(FString::Printf(TEXT("%s used %s! Your composure drops by %d."), *Def.Name, *AttackName, Damage));

	if (Battle.Heart <= 0)
	{
		EndBattle(FString::Printf(TEXT("You got completely flustered! %s scampers off... for now."), *Def.Name));
	}
}

void AMQGameMode::EndBattle(const FString& PartingLine)
{
	if (AMemoryCreature* Creature = BattleCreature.Get())
	{
		Creature->SetInBattle(false);
	}
	BattleCreature = nullptr;

	Flow = EMQFlow::Explore;
	SetPlayerFrozen(false);
	ShowBanner(PartingLine, FString(), 3.5f);
}

void AMQGameMode::AddBattleLog(const FString& Line)
{
	Battle.Log.Add(Line);
	while (Battle.Log.Num() > 4)
	{
		Battle.Log.RemoveAt(0);
	}
}

void AMQGameMode::MarkCreatureCaught(int32 Index)
{
	if (CaughtFlags.IsValidIndex(Index))
	{
		CaughtFlags[Index] = true;
	}
}

// ---------------------------------------------------------------------------
// Collections
// ---------------------------------------------------------------------------

void AMQGameMode::CollectKeepsake(int32 KeepsakeIndex)
{
	const TArray<FMQKeepsakeDef>& Keepsakes = StoryData::Keepsakes();
	if (!Keepsakes.IsValidIndex(KeepsakeIndex) || Flow != EMQFlow::Explore)
	{
		return;
	}

	if (KeepsakeFlags.IsValidIndex(KeepsakeIndex))
	{
		KeepsakeFlags[KeepsakeIndex] = true;
	}

	const FMQKeepsakeDef& Def = Keepsakes[KeepsakeIndex];
	ShowMemoryCard(Def.MemoryTitle, FString::Printf(TEXT("Keepsake - %s"), *Def.Name), Def.MemoryText);
	Flow = EMQFlow::MemoryCard;
	SetPlayerFrozen(true);
}

void AMQGameMode::RememberRumor()
{
	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	for (int32 Index = 0; Index < Creatures.Num(); ++Index)
	{
		if (Creatures[Index].bSpecial)
		{
			MarkCreatureCaught(Index);
			ShowMemoryCard(Creatures[Index].MemoryTitle,
				FString::Printf(TEXT("%s - %s"), *Creatures[Index].Name, *Creatures[Index].Species),
				Creatures[Index].MemoryText);
			Flow = EMQFlow::MemoryCard;
			SetPlayerFrozen(true);
			return;
		}
	}
}

int32 AMQGameMode::GetCaughtCount() const
{
	int32 Count = 0;
	for (const bool bCaught : CaughtFlags)
	{
		Count += bCaught ? 1 : 0;
	}
	return Count;
}

int32 AMQGameMode::GetKeepsakeCount() const
{
	int32 Count = 0;
	for (const bool bCollected : KeepsakeFlags)
	{
		Count += bCollected ? 1 : 0;
	}
	return Count;
}

int32 AMQGameMode::GetTotalCreatures() const
{
	return StoryData::Creatures().Num();
}

int32 AMQGameMode::GetTotalKeepsakes() const
{
	return StoryData::Keepsakes().Num();
}

// ---------------------------------------------------------------------------
// Zones / banner
// ---------------------------------------------------------------------------

void AMQGameMode::AnnounceZone(const FString& ZoneName, const FString& Subtitle)
{
	if (ZoneName == CurrentZoneName)
	{
		return;
	}
	CurrentZoneName = ZoneName;
	ShowBanner(ZoneName, Subtitle, 4.f);
}

void AMQGameMode::ShowBanner(const FString& Title, const FString& Subtitle, float Duration)
{
	BannerTitle = Title;
	BannerSubtitle = Subtitle;
	BannerHideTime = GetWorld()->GetTimeSeconds() + Duration;
}

// ---------------------------------------------------------------------------
// Escape room
// ---------------------------------------------------------------------------

bool AMQGameMode::IsEscapeRoomUnlocked() const
{
	return GetCaughtCount() >= StoryData::CreaturesNeededForEscapeRoom()
		&& GetKeepsakeCount() >= StoryData::KeepsakesNeededForEscapeRoom();
}

void AMQGameMode::OnEscapeDoorOpened()
{
	ShowBanner(TEXT("The Final Room"), TEXT("No mimosas this time. You've got this."), 4.f);
}

void AMQGameMode::RegisterPedestal(APuzzlePedestal* Pedestal)
{
	Pedestals.Add(Pedestal);
}

void AMQGameMode::ActivatePedestal(APuzzlePedestal* Pedestal)
{
	if (bPuzzleSolved || Pedestal == nullptr)
	{
		return;
	}

	const int32 Index = Pedestal->GetPuzzleIndex();
	if (Index < NextPedestalIndex)
	{
		return; // Already pressed; nothing happens.
	}

	if (Index == NextPedestalIndex)
	{
		Pedestal->MarkSolved();
		++NextPedestalIndex;

		if (NextPedestalIndex >= Pedestals.Num())
		{
			bPuzzleSolved = true;
			ShowBanner(StoryData::PuzzleSolvedText(), FString(), 5.f);
			GetWorld()->SpawnActor<ALevelTrophy>(ALevelTrophy::StaticClass(), TrophySpawnPoint, FRotator::ZeroRotator);
		}
		return;
	}

	// Wrong order: reset, with commentary.
	NextPedestalIndex = 0;
	for (APuzzlePedestal* Each : Pedestals)
	{
		if (Each != nullptr)
		{
			Each->ResetPedestal();
		}
	}
	StartDialogue(TEXT("The Final Room"), { StoryData::PuzzleWrongOrderText() }, FLinearColor(0.70f, 0.10f, 0.12f));
}

void AMQGameMode::StartFinale()
{
	if (Flow != EMQFlow::Explore)
	{
		return;
	}
	Flow = EMQFlow::Finale;
	SetPlayerFrozen(true);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void AMQGameMode::SetPlayerFrozen(bool bFrozen)
{
	if (AMQCharacter* Player = Cast<AMQCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->SetFrozen(bFrozen);
	}
}

void AMQGameMode::ShowMemoryCard(const FString& Title, const FString& Subtitle, const FString& Text)
{
	MemoryCard.Title = Title;
	MemoryCard.Subtitle = Subtitle;
	MemoryCard.Text = Text;
}
