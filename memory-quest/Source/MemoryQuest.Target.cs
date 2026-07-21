using UnrealBuildTool;

public class MemoryQuestTarget : TargetRules
{
	public MemoryQuestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		// Latest resolves to the engine's own current settings (V7 / Unreal5_8 on
		// UE 5.8) and keeps the project buildable on other 5.x versions too.
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MemoryQuest");
	}
}
