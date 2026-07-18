using UnrealBuildTool;

public class MemoryQuestEditorTarget : TargetRules
{
	public MemoryQuestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MemoryQuest");
	}
}
