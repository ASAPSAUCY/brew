using UnrealBuildTool;

public class MemoryQuestTarget : TargetRules
{
	public MemoryQuestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("MemoryQuest");
	}
}
