#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlatformerHUD.generated.h"

/**
 * Canvas-drawn HUD (no UMG assets): coin count, lives, and the win/lose banner.
 */
UCLASS()
class UNREALPLATFORMER_API APlatformerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawCenteredText(const FString& Text, const FLinearColor& Color, float CenterYFraction, float Scale);
};
