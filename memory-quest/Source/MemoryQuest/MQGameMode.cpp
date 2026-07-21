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

	// Buddy is always the starter partner; caught memories join behind him.
	Party.Add(INDEX_NONE);

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
	case 1: // Partner's reliable move.
	{
		const int32 Damage = FMath::RandRange(18, 30);
		Battle.HP = FMath::Max(1, Battle.HP - Damage);
		AddBattleLog(FString::Printf(TEXT("%s used %s! %s's spirit drops by %d."),
			*Battle.PartnerName, *Battle.PartnerAttackA, *Def.Name, Damage));
		if (Battle.HP == 1)
		{
			AddBattleLog(FString::Printf(TEXT("%s is all tired out! Treat time!"), *Def.Name));
		}
		CreatureTurn();
		break;
	}
	case 2: // Partner's wild move: bigger swing, less reliable.
	{
		const int32 Damage = FMath::RandRange(6, 44);
		Battle.HP = FMath::Max(1, Battle.HP - Damage);
		if (Damage < 14)
		{
			AddBattleLog(FString::Printf(TEXT("%s used %s... it kind of whiffed. %d spirit."),
				*Battle.PartnerName, *Battle.PartnerAttackB, Damage));
		}
		else
		{
			AddBattleLog(FString::Printf(TEXT("%s used %s! Huge! %s's spirit drops by %d."),
				*Battle.PartnerName, *Battle.PartnerAttackB, *Def.Name, Damage));
		}
		if (Battle.HP == 1)
		{
			AddBattleLog(FString::Printf(TEXT("%s is all tired out! Treat time!"), *Def.Name));
		}
		CreatureTurn();
		break;
	}
	case 3: // Give a treat: the befriend attempt.
	{
		float Chance = 0.20f + 0.60f * (1.f - static_cast<float>(Battle.HP) / static_cast<float>(Battle.MaxHP));
		Chance = FMath::Min(Chance, 0.92f);

		if (FMath::FRand() < Chance)
		{
			AddBattleLog(FString::Printf(TEXT("%s takes the treat... and joins your team!"), *Def.Name));
			MarkCreatureCaught(Battle.CreatureIndex);

			if (AMemoryCreature* Creature = BattleCreature.Get())
			{
				Creature->Destroy();
			}
			BattleCreature = nullptr;

			ShowMemoryCard(Def.MemoryTitle,
				FString::Printf(TEXT("%s - %s - joined your team!"), *Def.Name, *Def.Species),
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

void AMQGameMode::OnCyclePartnerPressed()
{
	if (Flow != EMQFlow::Explore || Party.Num() == 0)
	{
		return;
	}

	ActivePartyIndex = (ActivePartyIndex + 1) % Party.Num();
	ShowBanner(FString::Printf(TEXT("Partner: %s!"), *GetPartyMemberName(Party[ActivePartyIndex])),
		Party.Num() == 1 ? TEXT("Catch more memories to grow the team") : FString(), 2.5f);
}

FString AMQGameMode::GetPartyMemberName(int32 PartyEntry) const
{
	if (PartyEntry == INDEX_NONE)
	{
		return StoryData::BuddyName();
	}
	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	return Creatures.IsValidIndex(PartyEntry) ? Creatures[PartyEntry].Name : FString();
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

	// Send out the active partner. Partners nap between battles, so full HP.
	const int32 PartyEntry = Party.IsValidIndex(ActivePartyIndex) ? Party[ActivePartyIndex] : INDEX_NONE;
	if (PartyEntry == INDEX_NONE)
	{
		Battle.PartnerName = StoryData::BuddyName();
		Battle.PartnerAttackA = StoryData::BuddyAttackA();
		Battle.PartnerAttackB = StoryData::BuddyAttackB();
		Battle.PartnerMaxHP = StoryData::BuddyMaxHP();
	}
	else
	{
		const FMQCreatureDef& PartnerDef = Creatures[PartyEntry];
		Battle.PartnerName = PartnerDef.Name;
		Battle.PartnerAttackA = PartnerDef.AttackA;
		Battle.PartnerAttackB = PartnerDef.AttackB;
		Battle.PartnerMaxHP = PartnerDef.MaxHP;
	}
	Battle.PartnerHP = Battle.PartnerMaxHP;

	AddBattleLog(FString::Printf(TEXT("A wild memory appeared: %s, the %s!"), *Def.Name, *Def.Species));
	AddBattleLog(FString::Printf(TEXT("Go, %s! Wear it down, then befriend it with a treat."), *Battle.PartnerName));
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
	const int32 Damage = FMath::RandRange(10, 18);
	Battle.PartnerHP -= Damage;
	AddBattleLog(FString::Printf(TEXT("%s used %s! %s takes %d."), *Def.Name, *AttackName, *Battle.PartnerName, Damage));

	if (Battle.PartnerHP <= 0)
	{
		EndBattle(FString::Printf(TEXT("%s is all tuckered out! %s scampers off... for now."), *Battle.PartnerName, *Def.Name));
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
	if (CaughtFlags.IsValidIndex(Index) && !CaughtFlags[Index])
	{
		CaughtFlags[Index] = true;
		Party.Add(Index);
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
