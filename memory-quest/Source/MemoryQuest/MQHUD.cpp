#include "MQHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MQCharacter.h"
#include "MQGameMode.h"
#include "MQInteractable.h"
#include "StoryData.h"

namespace
{
	const FLinearColor PanelColor(0.02f, 0.02f, 0.05f, 0.78f);
	const FLinearColor SoftWhite(0.95f, 0.95f, 0.92f);
	const FLinearColor Gold(1.f, 0.83f, 0.25f);
	const FLinearColor HeartRed(0.90f, 0.25f, 0.35f);
}

UFont* AMQHUD::GetFont() const
{
	return GEngine->GetLargeFont();
}

void AMQHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Canvas == nullptr)
	{
		return;
	}

	AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>();
	if (GameMode == nullptr)
	{
		return;
	}

	switch (GameMode->GetFlow())
	{
	case EMQFlow::Explore:
		DrawTracker(*GameMode);
		DrawPrompt();
		break;
	case EMQFlow::Dialogue:
		DrawTracker(*GameMode);
		DrawDialogue(*GameMode);
		break;
	case EMQFlow::Battle:
		DrawBattle(*GameMode);
		break;
	case EMQFlow::MemoryCard:
		DrawMemoryCard(*GameMode);
		break;
	case EMQFlow::Scrapbook:
		DrawScrapbook(*GameMode);
		break;
	case EMQFlow::Finale:
		DrawFinale();
		break;
	default:
		break;
	}

	// The banner draws on top of everything except full-screen states.
	if (GameMode->GetFlow() == EMQFlow::Explore || GameMode->GetFlow() == EMQFlow::Dialogue)
	{
		DrawBanner(*GameMode);
	}
}

// ---------------------------------------------------------------------------
// Pieces
// ---------------------------------------------------------------------------

void AMQHUD::DrawTracker(const AMQGameMode& GameMode)
{
	const FString Line1 = FString::Printf(TEXT("Memorydex %d / %d"), GameMode.GetCaughtCount(), GameMode.GetTotalCreatures());
	const FString Line2 = FString::Printf(TEXT("Keepsakes %d / %d"), GameMode.GetKeepsakeCount(), GameMode.GetTotalKeepsakes());
	const FString Line3 = GameMode.IsEscapeRoomUnlocked()
		? TEXT("The Final Room: OPEN")
		: TEXT("The Final Room: locked");

	UFont* Font = GetFont();
	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Line1, Width, Height, Font, 1.3f);

	const float X = Canvas->SizeX - 330.f;
	DrawRect(PanelColor, X - 15.f, 25.f, 320.f, 3.f * (Height + 8.f) + 20.f);
	DrawText(Line1, Gold, X, 35.f, Font, 1.3f);
	DrawText(Line2, SoftWhite, X, 35.f + Height + 8.f, Font, 1.3f);
	DrawText(Line3, GameMode.IsEscapeRoomUnlocked() ? FLinearColor(0.3f, 1.f, 0.4f) : FLinearColor(0.7f, 0.7f, 0.7f),
		X, 35.f + 2.f * (Height + 8.f), Font, 1.3f);

	DrawText(TEXT("WASD move - Space jump - E interact - Tab scrapbook"),
		FLinearColor(1.f, 1.f, 1.f, 0.45f), 30.f, Canvas->SizeY - 40.f, Font, 1.f);
}

void AMQHUD::DrawBanner(const AMQGameMode& GameMode)
{
	if (GetWorld()->GetTimeSeconds() > GameMode.GetBannerHideTime())
	{
		return;
	}

	DrawCentered(GameMode.GetBannerTitle(), Gold, 90.f, 2.6f);
	if (!GameMode.GetBannerSubtitle().IsEmpty())
	{
		DrawCentered(GameMode.GetBannerSubtitle(), SoftWhite, 145.f, 1.4f);
	}
}

void AMQHUD::DrawPrompt()
{
	const AMQCharacter* Player = Cast<AMQCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player == nullptr)
	{
		return;
	}

	AMQInteractable* Focused = Player->GetFocusedInteractable();
	if (Focused == nullptr)
	{
		return;
	}

	const FString Prompt = Focused->GetPrompt();
	if (Prompt.IsEmpty())
	{
		return;
	}

	UFont* Font = GetFont();
	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Prompt, Width, Height, Font, 1.5f);

	const float X = (Canvas->SizeX - Width) * 0.5f;
	const float Y = Canvas->SizeY - 130.f;
	DrawRect(PanelColor, X - 20.f, Y - 10.f, Width + 40.f, Height + 20.f);
	DrawText(Prompt, SoftWhite, X, Y, Font, 1.5f);
}

void AMQHUD::DrawDialogue(const AMQGameMode& GameMode)
{
	UFont* Font = GetFont();

	const float BoxWidth = FMath::Min(Canvas->SizeX - 120.f, 1100.f);
	const float BoxX = (Canvas->SizeX - BoxWidth) * 0.5f;
	const float BoxY = Canvas->SizeY - 280.f;

	DrawRect(PanelColor, BoxX, BoxY, BoxWidth, 230.f);
	DrawRect(GameMode.GetDialogueColor(), BoxX, BoxY, BoxWidth, 6.f);

	DrawText(GameMode.GetDialogueSpeaker(), GameMode.GetDialogueColor() * 0.5f + FLinearColor(0.5f, 0.5f, 0.5f),
		BoxX + 25.f, BoxY + 20.f, Font, 1.7f);

	float BottomY = 0.f;
	DrawWrapped(GameMode.GetDialoguePage(), SoftWhite, BoxX + 25.f, BoxY + 65.f, BoxWidth - 50.f, 1.35f, BottomY);

	DrawText(FString::Printf(TEXT("E - continue (%d/%d)"), GameMode.GetDialoguePageNumber(), GameMode.GetDialoguePageCount()),
		FLinearColor(1.f, 1.f, 1.f, 0.55f), BoxX + BoxWidth - 220.f, BoxY + 195.f, Font, 1.1f);
}

void AMQHUD::DrawBattle(const AMQGameMode& GameMode)
{
	const FMQBattleState& Battle = GameMode.GetBattle();
	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	if (!Creatures.IsValidIndex(Battle.CreatureIndex))
	{
		return;
	}
	const FMQCreatureDef& Def = Creatures[Battle.CreatureIndex];

	UFont* Font = GetFont();

	// Wild memory panel (top left): name and spirit bar.
	DrawRect(PanelColor, 30.f, 30.f, 480.f, 120.f);
	DrawText(FString::Printf(TEXT("%s - %s"), *Def.Name, *Def.Species), Gold, 50.f, 45.f, Font, 1.5f);
	DrawText(TEXT("Spirit"), SoftWhite, 50.f, 90.f, Font, 1.f);
	const float SpiritFraction = FMath::Clamp(static_cast<float>(Battle.HP) / static_cast<float>(Battle.MaxHP), 0.f, 1.f);
	DrawRect(FLinearColor(0.2f, 0.2f, 0.2f), 130.f, 92.f, 340.f, 18.f);
	DrawRect(FLinearColor(0.35f, 0.85f, 0.45f), 130.f, 92.f, 340.f * SpiritFraction, 18.f);

	// Composure panel (bottom left of the option box).
	const float BoxWidth = FMath::Min(Canvas->SizeX - 120.f, 1100.f);
	const float BoxX = (Canvas->SizeX - BoxWidth) * 0.5f;
	const float BoxY = Canvas->SizeY - 300.f;

	// Battle log above the options.
	float LogY = BoxY - 40.f - 34.f * Battle.Log.Num();
	for (const FString& Line : Battle.Log)
	{
		DrawText(Line, SoftWhite, BoxX + 10.f, LogY, Font, 1.2f);
		LogY += 34.f;
	}

	DrawRect(PanelColor, BoxX, BoxY, BoxWidth, 250.f);
	DrawText(TEXT("Your composure"), SoftWhite, BoxX + 25.f, BoxY + 20.f, Font, 1.f);
	const float HeartFraction = FMath::Clamp(static_cast<float>(Battle.Heart) / 100.f, 0.f, 1.f);
	DrawRect(FLinearColor(0.2f, 0.2f, 0.2f), BoxX + 220.f, BoxY + 22.f, 300.f, 18.f);
	DrawRect(HeartRed, BoxX + 220.f, BoxY + 22.f, 300.f * HeartFraction, 18.f);

	DrawText(TEXT("1 - Tease it   (its resolve drops)"), SoftWhite, BoxX + 25.f, BoxY + 65.f, Font, 1.35f);
	DrawText(TEXT("2 - Sweet-talk it   (it likes you more)"), SoftWhite, BoxX + 25.f, BoxY + 105.f, Font, 1.35f);
	DrawText(TEXT("3 - Give it a treat   (try to befriend it!)"), Gold, BoxX + 25.f, BoxY + 145.f, Font, 1.35f);
	DrawText(TEXT("4 - Wave goodbye"), FLinearColor(0.7f, 0.7f, 0.7f), BoxX + 25.f, BoxY + 185.f, Font, 1.35f);
}

void AMQHUD::DrawMemoryCard(const AMQGameMode& GameMode)
{
	const FMQMemoryCard& Card = GameMode.GetMemoryCard();

	const float CardWidth = FMath::Min(Canvas->SizeX - 160.f, 900.f);
	const float CardHeight = 420.f;
	const float CardX = (Canvas->SizeX - CardWidth) * 0.5f;
	const float CardY = (Canvas->SizeY - CardHeight) * 0.5f;

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), 0.f, 0.f, Canvas->SizeX, Canvas->SizeY);
	DrawRect(FLinearColor(0.07f, 0.06f, 0.10f, 0.96f), CardX, CardY, CardWidth, CardHeight);
	DrawRect(Gold, CardX, CardY, CardWidth, 8.f);

	DrawCentered(TEXT("A MEMORY, KEPT"), FLinearColor(1.f, 1.f, 1.f, 0.5f), CardY + 30.f, 1.1f);
	DrawCentered(Card.Title, Gold, CardY + 70.f, 2.2f);
	DrawCentered(Card.Subtitle, FLinearColor(0.7f, 0.85f, 0.9f), CardY + 130.f, 1.2f);

	float BottomY = 0.f;
	DrawWrapped(Card.Text, SoftWhite, CardX + 60.f, CardY + 180.f, CardWidth - 120.f, 1.4f, BottomY);

	DrawCentered(TEXT("E - keep it forever"), FLinearColor(1.f, 1.f, 1.f, 0.55f), CardY + CardHeight - 45.f, 1.1f);
}

void AMQHUD::DrawScrapbook(const AMQGameMode& GameMode)
{
	UFont* Font = GetFont();

	DrawRect(FLinearColor(0.03f, 0.03f, 0.06f, 0.93f), 0.f, 0.f, Canvas->SizeX, Canvas->SizeY);
	DrawCentered(TEXT("THE SCRAPBOOK"), Gold, 45.f, 2.2f);
	DrawCentered(TEXT("Tab - close"), FLinearColor(1.f, 1.f, 1.f, 0.5f), Canvas->SizeY - 50.f, 1.1f);

	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	const TArray<FMQKeepsakeDef>& Keepsakes = StoryData::Keepsakes();

	const float LeftX = Canvas->SizeX * 0.08f;
	const float RightX = Canvas->SizeX * 0.56f;
	float Y = 130.f;

	DrawText(TEXT("MEMORYDEX"), FLinearColor(0.7f, 0.85f, 0.9f), LeftX, Y, Font, 1.5f);
	float LineY = Y + 45.f;
	for (int32 Index = 0; Index < Creatures.Num(); ++Index)
	{
		const FMQCreatureDef& Def = Creatures[Index];
		if (GameMode.IsCreatureCaught(Index))
		{
			DrawText(FString::Printf(TEXT("%s - %s"), *Def.Name, *Def.MemoryTitle), SoftWhite, LeftX, LineY, Font, 1.15f);
		}
		else
		{
			DrawText(FString::Printf(TEXT("??? - somewhere in %s"), *Def.Zone), FLinearColor(0.5f, 0.5f, 0.55f), LeftX, LineY, Font, 1.15f);
		}
		LineY += 32.f;
	}

	DrawText(TEXT("KEEPSAKES"), FLinearColor(0.7f, 0.85f, 0.9f), RightX, Y, Font, 1.5f);
	LineY = Y + 45.f;
	for (int32 Index = 0; Index < Keepsakes.Num(); ++Index)
	{
		const FMQKeepsakeDef& Def = Keepsakes[Index];
		if (GameMode.IsKeepsakeCollected(Index))
		{
			DrawText(FString::Printf(TEXT("%s - %s"), *Def.Name, *Def.MemoryTitle), SoftWhite, RightX, LineY, Font, 1.15f);
		}
		else
		{
			DrawText(TEXT("??? - keep exploring"), FLinearColor(0.5f, 0.5f, 0.55f), RightX, LineY, Font, 1.15f);
		}
		LineY += 32.f;
	}

	const FString Progress = GameMode.IsEscapeRoomUnlocked()
		? TEXT("The Final Room is OPEN. Go finish what you started.")
		: FString::Printf(TEXT("The Final Room opens at %d memories and %d keepsakes."),
			StoryData::CreaturesNeededForEscapeRoom(), StoryData::KeepsakesNeededForEscapeRoom());
	DrawCentered(Progress, Gold, Canvas->SizeY - 100.f, 1.25f);
}

void AMQHUD::DrawFinale()
{
	DrawRect(FLinearColor(0.02f, 0.02f, 0.05f, 0.97f), 0.f, 0.f, Canvas->SizeX, Canvas->SizeY);

	const TArray<FString>& Lines = StoryData::FinaleLines();
	float Y = Canvas->SizeY * 0.18f;
	for (int32 Index = 0; Index < Lines.Num(); ++Index)
	{
		if (Index == 0)
		{
			DrawCentered(Lines[Index], Gold, Y, 3.f);
			Y += 110.f;
		}
		else
		{
			float BottomY = Y;
			const float TextWidth = FMath::Min(Canvas->SizeX - 300.f, 1000.f);
			DrawWrapped(Lines[Index], SoftWhite, (Canvas->SizeX - TextWidth) * 0.5f, Y, TextWidth, 1.4f, BottomY);
			Y = BottomY + 34.f;
		}
	}
}

// ---------------------------------------------------------------------------
// Text helpers
// ---------------------------------------------------------------------------

void AMQHUD::DrawCentered(const FString& Text, const FLinearColor& Color, float Y, float Scale)
{
	UFont* Font = GetFont();
	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Text, Width, Height, Font, Scale);
	DrawText(Text, Color, (Canvas->SizeX - Width) * 0.5f, Y, Font, Scale);
}

TArray<FString> AMQHUD::WrapText(const FString& Text, float MaxWidth, float Scale) const
{
	TArray<FString> Words;
	Text.ParseIntoArray(Words, TEXT(" "), true);

	UFont* Font = GetFont();
	TArray<FString> Lines;
	FString Current;

	for (const FString& Word : Words)
	{
		const FString Candidate = Current.IsEmpty() ? Word : Current + TEXT(" ") + Word;
		float Width = 0.f;
		float Height = 0.f;
		GetTextSize(Candidate, Width, Height, Font, Scale);
		if (Width > MaxWidth && !Current.IsEmpty())
		{
			Lines.Add(Current);
			Current = Word;
		}
		else
		{
			Current = Candidate;
		}
	}
	if (!Current.IsEmpty())
	{
		Lines.Add(Current);
	}
	return Lines;
}

void AMQHUD::DrawWrapped(const FString& Text, const FLinearColor& Color, float X, float Y, float MaxWidth, float Scale, float& OutBottomY)
{
	UFont* Font = GetFont();
	float LineY = Y;
	for (const FString& Line : WrapText(Text, MaxWidth, Scale))
	{
		DrawText(Line, Color, X, LineY, Font, Scale);
		float Width = 0.f;
		float Height = 0.f;
		GetTextSize(Line, Width, Height, Font, Scale);
		LineY += Height + 6.f;
	}
	OutBottomY = LineY;
}
