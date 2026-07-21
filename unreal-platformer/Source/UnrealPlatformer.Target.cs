using UnrealBuildTool;

public class UnrealPlatformerTarget : TargetRules
{
	public UnrealPlatformerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("UnrealPlatformer");
	}
}
