#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Misc/MessageDialog.h"
#include "EmberImporter.h"
class FEmberNiagaraModule : public IModuleInterface {
public:
    virtual void StartupModule() override {
        if (!IsRunningCommandlet()) UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEmberNiagaraModule::RegisterMenu));
    }
    virtual void ShutdownModule() override {
        if (UToolMenus::IsToolMenuUIEnabled()) {UToolMenus::UnRegisterStartupCallback(this);UToolMenus::UnregisterOwner(this);}
    }
    void RegisterMenu() {
        FToolMenuOwnerScoped Owner(this);
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
        Menu->FindOrAddSection("Ember").AddMenuEntry("EmberImport", FText::FromString("Import Ember VFX…"), FText::FromString("Create a Niagara system from an Ember project."), FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FEmberNiagaraModule::ChooseFile)));
    }
    void ChooseFile() {
        TArray<FString> Files;
        if (FDesktopPlatformModule::Get()->OpenFileDialog(nullptr, TEXT("Import Ember VFX"), TEXT(""), TEXT(""), TEXT("Ember project (*.json)|*.json"), 0, Files) && Files.Num()) {
            FString Asset, Error;
            const bool Success = FEmberImporter::Import(Files[0], Asset, Error);
            FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Success ? TEXT("Niagara system created: ") + Asset : Error));
        }
    }
};
IMPLEMENT_MODULE(FEmberNiagaraModule, EmberNiagara)
