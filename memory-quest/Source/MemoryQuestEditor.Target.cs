using UnrealBuildTool;

public class MemoryQuestEditorTarget : TargetRules
{
	public MemoryQuestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("MemoryQuest");
	}
}
