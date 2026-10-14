#include "EmberImporter.h"
#include "EmberAnimation.h"
#include "EmberAdvanced.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "NiagaraScript.h"
#include "NiagaraEmitterFactoryNew.h"
#include "NiagaraSystemFactoryNew.h"
#include "NiagaraEditorUtilities.h"
#include "NiagaraScriptSource.h"
#include "NiagaraGraph.h"
#include "NiagaraNodeOutput.h"
#include "NiagaraNodeFunctionCall.h"
#include "NiagaraNodeAssignment.h"
#include "NiagaraConstants.h"
#include "NiagaraSpriteRendererProperties.h"
#include "ViewModels/Stack/NiagaraStackGraphUtilities.h"

namespace {
UNiagaraNodeOutput* Output(UNiagaraGraph* Graph, ENiagaraScriptUsage Usage) {
    for (UEdGraphNode* Node : Graph->Nodes) if (auto* Out=Cast<UNiagaraNodeOutput>(Node)) if (Out->GetUsage()==Usage) return Out;
    return nullptr;
}
template<typename T> void SetInput(UNiagaraEmitter* Emitter, UNiagaraScript* Script, UNiagaraNodeFunctionCall* Node, const TCHAR* Input, const FNiagaraTypeDefinition& Type, const T& Value) {
    FNiagaraVariable Var(Type,FName(*(Node->GetFunctionName()+TEXT(".")+Input)));
    Var.SetName(FName(*(TEXT("Constants.")+Emitter->GetUniqueEmitterName()+TEXT(".")+Node->GetFunctionName()+TEXT(".")+Input)));
    Var.SetValue(Value); Script->RapidIterationParameters.SetParameterData(Var.GetData(),Var,true);
}
UNiagaraNodeFunctionCall* AddModule(const TCHAR* Path, UNiagaraNodeOutput* Target) {
    auto* Script=LoadObject<UNiagaraScript>(nullptr,Path);
    return Script && Target ? FNiagaraStackGraphUtilities::AddScriptModuleToStack(Script,*Target) : nullptr;
}
bool ReadNumber(const TSharedPtr<FJsonObject>& Obj,const TCHAR* Key,double Min,double Max,double& Result) {
    return Obj->TryGetNumberField(Key,Result)&&FMath::IsFinite(Result)&&Result>=Min&&Result<=Max;
}
}
bool FEmberImporter::Import(const FString& Filename,FString& AssetPath,FString& Error) {
    FString Text;TSharedPtr<FJsonObject> Root;
    if(!FFileHelper::LoadFileToString(Text,*Filename)||Text.Len()>1024*1024||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Root)||!Root.IsValid()){Error=TEXT("Invalid project file.");return false;}
    FString Format,Name;double Version;const TSharedPtr<FJsonObject>* Effect;
    if(!Root->TryGetStringField(TEXT("format"),Format)||Format!=TEXT("ember-vfx")||!Root->TryGetNumberField(TEXT("version"),Version)||(Version!=1&&Version!=2)||!Root->TryGetStringField(TEXT("name"),Name)||Name.Len()>100||!Root->TryGetObjectField(TEXT("effect"),Effect)){Error=TEXT("Expected an Ember v1 project.");return false;}
    if(Version==2){
        const TArray<TSharedPtr<FJsonValue>>* Layers=nullptr;if(!Root->TryGetArrayField(TEXT("layers"),Layers)||Layers->Num()<1||Layers->Num()>8){Error=TEXT("Expected one to eight layers.");return false;}
        FString Safe;for(TCHAR C:Name)Safe.AppendChar(FChar::IsAlnum(C)&&C<128?C:'_');if(Safe.IsEmpty())Safe=TEXT("Effect");const FString Base=TEXT("/Game/Ember/NS_")+Safe;FString PackageName=Base;int32 Suffix=1;while(FPackageName::DoesPackageExist(PackageName)||FindPackage(nullptr,*PackageName))PackageName=Base+FString::Printf(TEXT("_%d"),Suffix++);
        auto* Package=CreatePackage(*PackageName);const FName AssetName(*FPackageName::GetLongPackageAssetName(PackageName));auto* System=NewObject<UNiagaraSystem>(Package,AssetName,RF_Public|RF_Standalone|RF_Transactional);UNiagaraSystemFactoryNew::InitializeSystem(System,true);int32 Enabled=0;
        for(const auto& Value:*Layers){const auto Layer=Value->AsObject();if(!Layer.IsValid()){Error=TEXT("Invalid layer.");return false;}bool Visible;const TSharedPtr<FJsonObject>* Settings=nullptr;if(!Layer->TryGetBoolField(TEXT("enabled"),Visible)||!Layer->TryGetObjectField(TEXT("effect"),Settings)){Error=TEXT("Invalid layer settings.");return false;}if(!Visible)continue;
            FString Motion;if((*Settings)->HasField(TEXT("motion"))&&(!(*Settings)->TryGetStringField(TEXT("motion"),Motion)||!TArray<FString>{TEXT("preset"),TEXT("aimed"),TEXT("orbit")}.Contains(Motion))){Error=TEXT("Invalid particle movement.");return false;}
            for(const auto& Range:TArray<TTuple<FString,double,double>>{{TEXT("trailLength"),.1,10},{TEXT("radiusVariation"),0,100},{TEXT("emitScaleX"),.1,10},{TEXT("emitScaleY"),.1,10},{TEXT("emitScaleZ"),.1,10},{TEXT("spinVariation"),0,100},{TEXT("directionRandom"),0,180},{TEXT("spriteRotationRandom"),0,180},{TEXT("velocityYaw"),-180,180},{TEXT("velocityPitch"),-90,90},{TEXT("ringSpin"),-360,360}})if((*Settings)->HasField(*Range.Get<0>())){double AngleValue;if(!ReadNumber(*Settings,*Range.Get<0>(),Range.Get<1>(),Range.Get<2>(),AngleValue)){Error=TEXT("Invalid velocity angle or ring spin.");return false;}}
            if(!EmberValidateAnimation(*Settings,Error))return false;
            for(const TCHAR* Key:{TEXT("positionX"),TEXT("positionY"),TEXT("positionZ")})if((*Settings)->HasField(Key)){double Offset;if(!ReadNumber(*Settings,Key,-10000,10000,Offset)){Error=TEXT("Invalid starting position.");return false;}}
            double Number;for(const auto& Range:TArray<TTuple<FString,double,double>>{{TEXT("amount"),5,200},{TEXT("size"),1,30},{TEXT("speed"),1,100},{TEXT("spread"),5,100},{TEXT("life"),1,8},{TEXT("gravity"),-100,100},{TEXT("turbulence"),0,100},{TEXT("endSize"),0,5},{TEXT("endOpacity"),0,1}})if(!ReadNumber(*Settings,*Range.Get<0>(),Range.Get<1>(),Range.Get<2>(),Number)){Error=TEXT("Invalid numeric setting.");return false;}
            FString String;for(const auto& Key:TArray<FString>{TEXT("id"),TEXT("color"),TEXT("shape"),TEXT("sprite"),TEXT("mode")})if(!(*Settings)->TryGetStringField(Key,String)){Error=TEXT("Missing particle setting.");return false;}bool Flag;for(const auto& Key:TArray<FString>{TEXT("glow"),TEXT("trail"),TEXT("bounce")})if(!(*Settings)->TryGetBoolField(Key,Flag)){Error=TEXT("Missing particle behavior.");return false;}
            const FString Shape=(*Settings)->GetStringField(TEXT("shape")),Sprite=(*Settings)->GetStringField(TEXT("sprite")),Mode=(*Settings)->GetStringField(TEXT("mode")),Hex=(*Settings)->GetStringField(TEXT("color"));
            if(!TArray<FString>{TEXT("point"),TEXT("sphere"),TEXT("cone"),TEXT("ring"),TEXT("box")}.Contains(Shape)||!TArray<FString>{TEXT("soft"),TEXT("star"),TEXT("square"),TEXT("diamond"),TEXT("ring")}.Contains(Sprite)||(Mode!=TEXT("continuous")&&Mode!=TEXT("burst"))||Hex.Len()!=7||Hex[0]!='#'){Error=TEXT("Invalid appearance or emission setting.");return false;}for(int32 I=1;I<Hex.Len();I++)if(!FChar::IsHexDigit(Hex[I])){Error=TEXT("Invalid color.");return false;}
            FString LayerName;Layer->TryGetStringField(TEXT("name"),LayerName);FString SafeLayer;for(TCHAR C:LayerName.Left(60))SafeLayer.AppendChar(FChar::IsAlnum(C)&&C<128?C:'_');if(SafeLayer.IsEmpty())SafeLayer=TEXT("Layer");SafeLayer+=FString::Printf(TEXT("_%d"),++Enabled);
            if(!EmberAddLayer(System,*Settings,SafeLayer,Error))return false;
        }
        if(!Enabled){Error=TEXT("Enable at least one particle layer before exporting.");return false;}
        System->RequestCompile(false);System->WaitForCompilationComplete();TArray<UNiagaraScript*> Scripts={System->GetSystemSpawnScript(),System->GetSystemUpdateScript()};for(const auto& Handle:System->GetEmitterHandles()){auto* Data=Handle.GetEmitterData();Scripts.Add(Data->SpawnScriptProps.Script);Scripts.Add(Data->UpdateScriptProps.Script);}
        for(auto* Script:Scripts){const auto Status=Script->GetLastCompileStatus();if(Status!=ENiagaraScriptCompileStatus::NCS_UpToDate&&Status!=ENiagaraScriptCompileStatus::NCS_UpToDateWithWarnings){Error=Script->GetVMExecutableData().ErrorMsg;for(const auto& Event:Script->GetVMExecutableData().LastCompileEvents){Error+=Event.Message+TEXT("\n");}UE_LOG(LogTemp,Error,TEXT("EMBER_COMPILE %s"),*Error);return false;}}
        FAssetRegistryModule::AssetCreated(System);System->MarkPackageDirty();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(PackageName,FPackageName::GetAssetPackageExtension());if(!UPackage::SavePackage(Package,System,*File,Args)){Error=TEXT("Could not save Niagara system.");return false;}AssetPath=PackageName+TEXT(".")+AssetName.ToString();return true;
    }
    const auto& E=*Effect;FString Id,Hex,Mode;double Amount,Size,Speed,Spread,Life;bool Glow,Trail;
    const TArray<FString> Presets={TEXT("cosmic"),TEXT("fire"),TEXT("magic"),TEXT("smoke"),TEXT("snow"),TEXT("ember")};
    if(!E->TryGetStringField(TEXT("id"),Id)||!Presets.Contains(Id)||!E->TryGetStringField(TEXT("color"),Hex)||Hex.Len()!=7||Hex[0]!='#'||!E->TryGetStringField(TEXT("mode"),Mode)||(Mode!=TEXT("burst")&&Mode!=TEXT("continuous"))||!E->TryGetBoolField(TEXT("glow"),Glow)||!E->TryGetBoolField(TEXT("trail"),Trail)||!ReadNumber(E,TEXT("amount"),5,200,Amount)||!ReadNumber(E,TEXT("size"),1,30,Size)||!ReadNumber(E,TEXT("speed"),1,100,Speed)||!ReadNumber(E,TEXT("spread"),5,100,Spread)||!ReadNumber(E,TEXT("life"),1,8,Life)){Error=TEXT("Invalid particle settings.");return false;}
    for(int32 I=1;I<Hex.Len();++I)if(!FChar::IsHexDigit(Hex[I])){Error=TEXT("Invalid color.");return false;}
    FString Safe;for(TCHAR C:Name)Safe.AppendChar(FChar::IsAlnum(C)&&C<128?C:'_');if(Safe.IsEmpty())Safe=TEXT("Effect");
    const FString Base=TEXT("/Game/Ember/NS_")+Safe;FString PackageName=Base;int32 Suffix=1;
    while(FPackageName::DoesPackageExist(PackageName)||FindPackage(nullptr,*PackageName))PackageName=Base+FString::Printf(TEXT("_%d"),Suffix++);
    UPackage* Package=CreatePackage(*PackageName);FName AssetName(*FPackageName::GetLongPackageAssetName(PackageName));
    auto* System=NewObject<UNiagaraSystem>(Package,AssetName,RF_Public|RF_Standalone|RF_Transactional);
    UNiagaraSystemFactoryNew::InitializeSystem(System,true);
    auto* Emitter=NewObject<UNiagaraEmitter>(GetTransientPackage(),NAME_None,RF_Transactional);
    UNiagaraEmitterFactoryNew::InitializeEmitter(Emitter,true);
    auto* Data=Emitter->GetLatestEmitterData();auto* Source=CastChecked<UNiagaraScriptSource>(Data->GraphSource);UNiagaraGraph* Graph=Source->NodeGraph.Get();
    UNiagaraNodeOutput* Spawn=Output(Graph,ENiagaraScriptUsage::ParticleSpawnScript);if(!Spawn)Spawn=Output(Graph,ENiagaraScriptUsage::ParticleSpawnScriptInterpolated);
    auto* Update=Output(Graph,ENiagaraScriptUsage::ParticleUpdateScript);auto* EmitterUpdate=Output(Graph,ENiagaraScriptUsage::EmitterUpdateScript);
    if(!Spawn||!Update||!EmitterUpdate){Error=TEXT("Engine Niagara defaults are unavailable.");return false;}
    for(UEdGraphNode* Node:Graph->Nodes)if(auto* Call=Cast<UNiagaraNodeFunctionCall>(Node))if(Call->FunctionScript){
        const FString Module=Call->FunctionScript->GetName();
        if(Module==TEXT("SpawnRate")){SetInput(Emitter,Data->EmitterUpdateScriptProps.Script,Call,TEXT("SpawnRate"),FNiagaraTypeDefinition::GetFloatDef(),float(Mode==TEXT("burst")?0:Amount));}
        if(Module==TEXT("Color"))FNiagaraStackGraphUtilities::SetModuleIsEnabled(*Call,false);
    }
    if(Mode==TEXT("burst")){
        auto* Burst=AddModule(TEXT("/Niagara/Modules/Emitter/SpawnBurst_Instantaneous.SpawnBurst_Instantaneous"),EmitterUpdate);
        if(!Burst){Error=TEXT("Niagara burst module is unavailable.");return false;}
        SetInput(Emitter,Data->EmitterUpdateScriptProps.Script,Burst,TEXT("Spawn Count"),FNiagaraTypeDefinition::GetIntDef(),int32(Amount*2));
        SetInput(Emitter,Data->EmitterUpdateScriptProps.Script,Burst,TEXT("Spawn Time"),FNiagaraTypeDefinition::GetFloatDef(),0.0f);
    }
    auto* Sphere=AddModule(TEXT("/Niagara/Modules/Spawn/Location/SphereLocation.SphereLocation"),Spawn);
    if(!Sphere){Error=TEXT("Niagara sphere location module is unavailable.");return false;}
    SetInput(Emitter,Data->SpawnScriptProps.Script,Sphere,TEXT("Sphere Radius"),FNiagaraTypeDefinition::GetFloatDef(),float(Spread));
    FLinearColor Color=FLinearColor::FromSRGBColor(FColor::FromHex(Hex));if(Glow){Color.R*=3;Color.G*=3;Color.B*=3;}Color.A=Id==TEXT("smoke")?0.18f:0.85f;
    const FVector3f Velocity(0,0,float(Speed*(Id==TEXT("snow")?-2:2)));
    const FVector2f SpriteSize(float(Size*2),float(Trail?Size*4:Size*2));
    TArray<FNiagaraVariable> Vars={SYS_PARAM_PARTICLES_LIFETIME,SYS_PARAM_PARTICLES_SPRITE_SIZE,SYS_PARAM_PARTICLES_COLOR,SYS_PARAM_PARTICLES_VELOCITY};
    TArray<FString> Values={FString::SanitizeFloat(Life),FString::Printf(TEXT("(X=%f,Y=%f)"),SpriteSize.X,SpriteSize.Y),FString::Printf(TEXT("(R=%f,G=%f,B=%f,A=%f)"),Color.R,Color.G,Color.B,Color.A),FString::Printf(TEXT("(X=%f,Y=%f,Z=%f)"),Velocity.X,Velocity.Y,Velocity.Z)};
    FNiagaraStackGraphUtilities::AddParameterModuleToStack(Vars,*Spawn,INDEX_NONE,Values);
    if(Trail)for(auto* Renderer:Data->GetRenderers())if(auto* Sprite=Cast<UNiagaraSpriteRendererProperties>(Renderer))Sprite->Alignment=ENiagaraSpriteAlignment::VelocityAligned;
    FNiagaraEditorUtilities::AddEmitterToSystem(*System,*Emitter,Data->Version.VersionGuid);
    System->RequestCompile(false);System->WaitForCompilationComplete();
    TArray<UNiagaraScript*> CompiledScripts={System->GetSystemSpawnScript(),System->GetSystemUpdateScript()};
    for(const auto& Handle:System->GetEmitterHandles())if(auto* Instance=Handle.GetEmitterData()){
        CompiledScripts.Add(Instance->SpawnScriptProps.Script);CompiledScripts.Add(Instance->UpdateScriptProps.Script);
    }
    for(auto* Script:CompiledScripts){
        const auto Status=Script->GetLastCompileStatus();
        UE_LOG(LogTemp,Display,TEXT("EMBER_SCRIPT %s status=%d %s"),*Script->GetName(),int32(Status),*Script->GetVMExecutableData().ErrorMsg);
        if(Status!=ENiagaraScriptCompileStatus::NCS_UpToDate&&Status!=ENiagaraScriptCompileStatus::NCS_UpToDateWithWarnings){Error=TEXT("Niagara compilation failed: ")+Script->GetVMExecutableData().ErrorMsg;return false;}
    }
    FAssetRegistryModule::AssetCreated(System);System->MarkPackageDirty();
    FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;
    const FString Target=FPackageName::LongPackageNameToFilename(PackageName,FPackageName::GetAssetPackageExtension());
    if(!UPackage::SavePackage(Package,System,*Target,Args)){Error=TEXT("Unable to save Niagara asset.");return false;}
    AssetPath=PackageName+TEXT(".")+AssetName.ToString();return true;
}
