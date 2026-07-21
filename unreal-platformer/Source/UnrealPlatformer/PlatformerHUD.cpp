#include "PlatformerHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "PlatformerGameMode.h"

void APlatformerHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Canvas == nullptr)
	{
		return;
	}

	const APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	if (GameMode == nullptr)
	{
		return;
	}

	UFont* Font = GEngine->GetLargeFont();

	DrawText(FString::Printf(TEXT("Coins: %d / %d"), GameMode->GetScore(), GameMode->GetTotalCoins()),
		FLinearColor(1.f, 0.85f, 0.1f), 40.f, 40.f, Font, 1.8f);

	const int32 Lives = GameMode->GetLives();
	DrawText(FString::Printf(TEXT("Lives: %d"), Lives),
		Lives > 1 ? FLinearColor::White : FLinearColor::Red, 40.f, 85.f, Font, 1.8f);

	switch (GameMode->GetPlatformerState())
	{
	case EPlatformerGameState::Won:
		DrawCenteredText(TEXT("YOU WIN!"), FLinearColor(0.2f, 1.f, 0.3f), 0.4f, 4.f);
		DrawCenteredText(TEXT("Press R to play again"), FLinearColor::White, 0.52f, 1.6f);
		break;
	case EPlatformerGameState::Lost:
		DrawCenteredText(TEXT("GAME OVER"), FLinearColor::Red, 0.4f, 4.f);
		DrawCenteredText(TEXT("Press R to try again"), FLinearColor::White, 0.52f, 1.6f);
		break;
	default:
		break;
	}
}

void APlatformerHUD::DrawCenteredText(const FString& Text, const FLinearColor& Color, float CenterYFraction, float Scale)
{
	UFont* Font = GEngine->GetLargeFont();

	float TextWidth = 0.f;
	float TextHeight = 0.f;
	GetTextSize(Text, TextWidth, TextHeight, Font, Scale);

	DrawText(Text, Color, (Canvas->SizeX - TextWidth) * 0.5f, Canvas->SizeY * CenterYFraction, Font, Scale);
}
