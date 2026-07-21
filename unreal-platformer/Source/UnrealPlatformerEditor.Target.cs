using UnrealBuildTool;

public class UnrealPlatformerEditorTarget : TargetRules
{
	public UnrealPlatformerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("UnrealPlatformer");
	}
}
