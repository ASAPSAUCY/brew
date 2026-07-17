#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MQHUD.generated.h"

class AMQGameMode;
class UFont;

/**
 * All UI, canvas-drawn (no UMG assets): interaction prompts, zone banners, the
 * quest tracker, dialogue boxes, the battle screen, memory cards, the
 * scrapbook, and the finale.
 */
UCLASS()
class MEMORYQUEST_API AMQHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawTracker(const AMQGameMode& GameMode);
	void DrawBanner(const AMQGameMode& GameMode);
	void DrawPrompt();
	void DrawDialogue(const AMQGameMode& GameMode);
	void DrawBattle(const AMQGameMode& GameMode);
	void DrawMemoryCard(const AMQGameMode& GameMode);
	void DrawScrapbook(const AMQGameMode& GameMode);
	void DrawFinale();

	void DrawCentered(const FString& Text, const FLinearColor& Color, float Y, float Scale);
	void DrawWrapped(const FString& Text, const FLinearColor& Color, float X, float Y, float MaxWidth, float Scale, float& OutBottomY);
	TArray<FString> WrapText(const FString& Text, float MaxWidth, float Scale) const;

	UFont* GetFont() const;
};
