using UnrealBuildTool;
public class EmberNiagara : ModuleRules {
    public EmberNiagara(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new [] {"Core", "CoreUObject", "Engine"});
        PrivateDependencyModuleNames.AddRange(new [] {"EmberRuntime", "NiagaraShader", "Niagara", "NiagaraEditor", "UnrealEd", "AssetRegistry", "Json", "Slate", "SlateCore", "ToolMenus", "DesktopPlatform", "AssetTools", "RHI", "RenderCore"});
    }
}
