using UnrealBuildTool;
public class EmberRuntime : ModuleRules {
 public EmberRuntime(ReadOnlyTargetRules Target):base(Target){PCHUsage=PCHUsageMode.UseExplicitOrSharedPCHs;PublicDependencyModuleNames.AddRange(new[]{"Core","CoreUObject","Engine","Niagara"});}
}
