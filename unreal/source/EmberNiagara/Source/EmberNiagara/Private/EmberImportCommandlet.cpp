#include "EmberImportCommandlet.h"
#include "EmberImporter.h"
#include "EmberMeshTransfer.h"
#include "EmberWorkspace.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "FXSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraEmitterInstance.h"
#include "NiagaraDataSetAccessor.h"
#include "NiagaraSpriteRendererProperties.h"
#include "NiagaraMeshRendererProperties.h"
#include "NiagaraParameterStore.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"

namespace {
bool VerifyTimeline(const FString& Asset,bool Orbit=false,bool Stretch=false,bool Distance=false,bool TrailLength=false) {
    auto* System=LoadObject<UNiagaraSystem>(nullptr,*Asset);
    if(!System||!System->IsReadyToRun()) {
        UE_LOG(LogTemp,Error,TEXT("EMBER_TIMELINE_TEST System is not ready to simulate"));
        return false;
    }
    auto* World=UWorld::CreateWorld(EWorldType::Game,false);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    // Commandlets skip normal world FX setup; this optional test needs a simulation world.
    World->FXSystem=FFXSystemInterface::Create(World->GetFeatureLevel(),World->Scene);
    auto* Component=NewObject<UNiagaraComponent>(World);
    Component->SetAutoActivate(false);
    Component->SetForceSolo(true);
    Component->SetAsset(System);
    Component->RegisterComponentWithWorld(World);
    Component->Activate(true);
    bool Valid=true;float PreviousAngle=0;bool SeenAngle=false;TArray<float> Radii;
    for(int32 Step=1;Step<=11;Step++) {
        Component->AdvanceSimulation(15,1.0f/60.0f);
        auto Controller=Component->GetSystemInstanceController();
        auto* Instance=Controller.IsValid()?Controller->GetSystemInstance_Unsafe():nullptr;
        const int32 Count=Instance&&Instance->GetEmitters().Num()>0?Instance->GetEmitters()[0]->GetNumParticles():-1;
        UE_LOG(LogTemp,Display,TEXT("EMBER_TIMELINE_SAMPLE %.2f %d"),Step*.25,Count);
        if(TrailLength){
            if(!Instance||Instance->GetEmitters().Num()!=3){Valid=false;continue;}
            for(int32 EmitterIndex=0;EmitterIndex<3;EmitterIndex++){
                auto& Emitter=Instance->GetEmitters()[EmitterIndex].Get();const auto& Data=Emitter.GetParticleData();
                const auto Sizes=FNiagaraDataSetAccessor<FVector2f>(Data,TEXT("SpriteSize")).GetReader(Data);
                const auto Positions=FNiagaraDataSetAccessor<FNiagaraPosition>(Data,TEXT("Position")).GetReader(Data);
                const auto Velocities=FNiagaraDataSetAccessor<FVector3f>(Data,TEXT("Velocity")).GetReader(Data);
                const int32 TrailCount=Emitter.GetNumParticles();if(TrailCount!=100||!Sizes.IsValid()||!Positions.IsValid()||!Velocities.IsValid()){Valid=false;continue;}
                const auto* EmitterData=System->GetEmitterHandles()[EmitterIndex].GetEmitterData();
                int32 Renderers=0;UNiagaraSpriteRendererProperties* TrailRenderer=nullptr;
                for(auto* Renderer:EmitterData->GetRenderers())if(auto* Sprite=Cast<UNiagaraSpriteRendererProperties>(Renderer)){Renderers++;if(Sprite->PositionBinding.GetParamMapBindableVariable().GetName()==FName(TEXT("Particles.EmberTrailPosition")))TrailRenderer=Sprite;}
                if(Renderers!=(EmitterIndex==1?1:2))Valid=false;
                for(int32 Index=0;Index<TrailCount;Index++){const auto Size=Sizes.Get(Index);if(FMath::Abs(Size.X-6.0f)>.01f||FMath::Abs(Size.Y-6.0f)>.01f)Valid=false;}
                if(EmitterIndex==1){if(TrailRenderer)Valid=false;continue;}
                if(!TrailRenderer||!TrailRenderer->Material||TrailRenderer->Alignment!=ENiagaraSpriteAlignment::VelocityAligned||TrailRenderer->SpriteSizeBinding.GetParamMapBindableVariable().GetName()!=FName(TEXT("Particles.EmberTrailSize"))||TrailRenderer->ColorBinding.GetParamMapBindableVariable().GetName()!=FName(TEXT("Particles.EmberTrailColor"))){Valid=false;continue;}
                const auto TailPositions=FNiagaraDataSetAccessor<FNiagaraPosition>(Data,TEXT("EmberTrailPosition")).GetReader(Data);
                const auto TailSizes=FNiagaraDataSetAccessor<FVector2f>(Data,TEXT("EmberTrailSize")).GetReader(Data);
                const auto TailColors=FNiagaraDataSetAccessor<FLinearColor>(Data,TEXT("EmberTrailColor")).GetReader(Data);
                if(!TailPositions.IsValid()||!TailSizes.IsValid()||!TailColors.IsValid()){Valid=false;continue;}
                const float LengthFactor=EmitterIndex==0&&Step>=6?5.0f:1.0f;
                if(Step<=3||Step>=6)for(int32 Index=0;Index<TrailCount;Index++){
                    const auto P=Positions.Get(Index),Tail=TailPositions.Get(Index);const auto V=Velocities.Get(Index);const auto Size=TailSizes.Get(Index);const auto Color=TailColors.Get(Index);
                    const FVector3f Offset(P.X-Tail.X,P.Y-Tail.Y,P.Z-Tail.Z),Expected=V*(.15f*LengthFactor*.5f);
                    if((Offset-Expected).Size()>.01f||FMath::Abs(Size.X-1.05f)>.01f||FMath::Abs(Size.Y-V.Size()*.15f*LengthFactor)>.01f||!FMath::IsFinite(Color.A)||Color.A<=0)Valid=false;
                }
                UE_LOG(LogTemp,Display,TEXT("EMBER_TRAIL_RENDERER_SAMPLE %d %.2f length %.3f"),EmitterIndex,Step*.25,TailSizes.Get(0).Y);
            }
        }else if(Orbit){
            if(Count!=100){Valid=false;continue;}
            const auto& Data=Instance->GetEmitters()[0]->GetParticleData();
            const auto Positions=FNiagaraDataSetAccessor<FNiagaraPosition>(Data,TEXT("Position")).GetReader(Data);
            const auto Velocities=FNiagaraDataSetAccessor<FVector3f>(Data,TEXT("Velocity")).GetReader(Data);
            if(!Positions.IsValid()||!Velocities.IsValid()){Valid=false;continue;}
            const float SX=Stretch?2.0f:1.0f,SY=Stretch?.5f:1.0f;float MinSpin=1000,MaxSpin=-1000,MinRotation=1000,MaxRotation=-1000,MinRadius=1000,MaxRadius=-1000;const auto Rotations=FNiagaraDataSetAccessor<float>(Data,TEXT("SpriteRotation")).GetReader(Data);
            for(int32 Index=0;Index<Count;Index++){const auto P=Positions.Get(Index);const auto V=Velocities.Get(Index);const float Radius=FMath::Sqrt(FMath::Square((P.X-100)/SX)+FMath::Square((P.Y+50)/SY));if(!FMath::IsFinite(Radius)||(Distance?(Radius<55.0f||Radius>128.5f):FMath::Abs(Radius-91.75f)>.1f)||FMath::Abs(P.Z-128)>.1f||FMath::Abs((P.X-100)/SX*V.X/SX+(P.Y+50)/SY*V.Y/SY)>.1f)Valid=false;if(Distance){MinRadius=FMath::Min(MinRadius,Radius);MaxRadius=FMath::Max(MaxRadius,Radius);if(Step==1)Radii.Add(Radius);else if(FMath::Abs(Radius-Radii[Index])>.1f)Valid=false;}if(Stretch){const float Spin=FMath::Abs(((P.X-100)/SX*V.Y/SY-(P.Y+50)/SY*V.X/SX)/(Radius*Radius)*180.0f/PI);MinSpin=FMath::Min(MinSpin,Spin);MaxSpin=FMath::Max(MaxSpin,Spin);const float Rotation=Rotations.GetSafe(Index,0);MinRotation=FMath::Min(MinRotation,Rotation);MaxRotation=FMath::Max(MaxRotation,Rotation);}}
            if(Distance&&MaxRadius-MinRadius<30)Valid=false;
            if(Stretch&&(MinSpin<44.9f||MaxSpin>135.1f||MaxSpin-MinSpin<30||MaxRotation-MinRotation<100))Valid=false;
            const auto P=Positions.Get(0);const float Angle=FMath::Atan2((P.Y+50)/SY,(P.X-100)/SX);const float Delta=FMath::Atan2(FMath::Sin(Angle-PreviousAngle),FMath::Cos(Angle-PreviousAngle));if(!Stretch&&SeenAngle&&Step<=5&&(Delta<.35f||Delta>.43f))Valid=false;if(!Stretch&&SeenAngle&&Step>=8&&(Delta>-.35f||Delta<-.43f))Valid=false;PreviousAngle=Angle;SeenAngle=true;
        }else if((Step==1&&Count!=0)||(Step==3&&Count!=10)||(Step==5&&(Count<18||Count>22))||
           (Step==7&&(Count<28||Count>32))||(Step==9&&(Count<38||Count>42))||(Step==11&&Count!=0))Valid=false;
    }
    Component->DestroyComponent();
    GEngine->DestroyWorldContext(World);
    World->DestroyWorld(false);
    if(Valid){UE_LOG(LogTemp,Display,TEXT("EMBER_TIMELINE_SIMULATION_SUCCESS"));}
    else{UE_LOG(LogTemp,Error,TEXT("EMBER_TIMELINE_SIMULATION_FAILED"));}
    return Valid;
}
}
UEmberImportCommandlet::UEmberImportCommandlet() { IsClient=false; IsServer=false; IsEditor=true; LogToConsole=true; }
int32 UEmberImportCommandlet::Main(const FString& Params) {
    FString Workspace,WorkspaceOutput,WorkspaceError;
    if(FParse::Value(*Params,TEXT("Workspace="),Workspace)){if(!FParse::Value(*Params,TEXT("WorkspaceOutput="),WorkspaceOutput)||!EmberWorkspace(Workspace,WorkspaceOutput,WorkspaceError)){UE_LOG(LogTemp,Error,TEXT("EMBER_WORKSPACE_FAILED %s"),*WorkspaceError);return 1;}UE_LOG(LogTemp,Display,TEXT("EMBER_WORKSPACE_SUCCESS"));return 0;}
    FString File, Asset, Error,MeshFile,MeshAsset,MeshOutput;
    if(FParse::Value(*Params,TEXT("MeshImport="),MeshFile)){if(!EmberImportMesh(MeshFile,Asset,Error)){UE_LOG(LogTemp,Error,TEXT("EMBER_MESH_FAILED %s"),*Error);return 1;}UE_LOG(LogTemp,Display,TEXT("EMBER_MESH_IMPORT_SUCCESS %s"),*Asset);return 0;}
    if(FParse::Value(*Params,TEXT("MeshExport="),MeshAsset)){if(!FParse::Value(*Params,TEXT("MeshOutput="),MeshOutput)||!EmberExportMesh(MeshAsset,MeshOutput,Error)){UE_LOG(LogTemp,Error,TEXT("EMBER_MESH_FAILED %s"),*Error);return 1;}UE_LOG(LogTemp,Display,TEXT("EMBER_MESH_EXPORT_SUCCESS"));return 0;}
    if(!FParse::Value(*Params,TEXT("Effect="),File)){UE_LOG(LogTemp,Error,TEXT("Missing -Effect=<Ember JSON file>"));return 1;}
    if(!FEmberImporter::Import(File,Asset,Error)){UE_LOG(LogTemp,Error,TEXT("EMBER_IMPORT_FAILED %s"),*Error);return 1;}
    if(FParse::Param(*Params,TEXT("VerifyWorkspaceFixture"))){
        auto* System=LoadObject<UNiagaraSystem>(nullptr,*Asset);auto& Store=System->GetExposedParameters();
        Store.SetParameterValue(2.0f,FNiagaraVariable(FNiagaraTypeDefinition::GetFloatDef(),TEXT("User.Intensity")),true);
        Store.SetParameterValue(int32(12),FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(),TEXT("User.Count")),true);
        FNiagaraBool Flag;Flag.SetValue(true);Store.SetParameterValue(Flag,FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(),TEXT("User.Active")),true);
        Store.SetParameterValue(FVector3f(1,2,3),FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(),TEXT("User.Direction")),true);
        Store.SetParameterValue(FLinearColor(1,.5f,.25f,1),FNiagaraVariable(FNiagaraTypeDefinition::GetColorDef(),TEXT("User.Tint")),true);
        auto* Package=System->GetOutermost();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;
        if(!UPackage::SavePackage(Package,System,*FPackageName::LongPackageNameToFilename(Package->GetName(),FPackageName::GetAssetPackageExtension()),Args))return 1;
    }
    if(FParse::Param(*Params,TEXT("VerifyMeshRenderer"))){auto* System=LoadObject<UNiagaraSystem>(nullptr,*Asset);if(!System||System->GetEmitterHandles().Num()!=1)return 1;int32 Meshes=0,Trails=0;for(auto* Renderer:System->GetEmitterHandles()[0].GetEmitterData()->GetRenderers()){if(auto* Mesh=Cast<UNiagaraMeshRendererProperties>(Renderer)){if(Mesh->Meshes.Num()!=1||!Mesh->Meshes[0].Mesh||Mesh->OverrideMaterials.IsEmpty()||!Mesh->OverrideMaterials[0].ExplicitMat)return 1;Meshes++;}else if(auto* Trail=Cast<UNiagaraSpriteRendererProperties>(Renderer)){if(Trail->PositionBinding.GetParamMapBindableVariable().GetName()!=FName(TEXT("Particles.EmberTrailPosition")))return 1;Trails++;}}if(Meshes!=1||Trails!=1)return 1;UE_LOG(LogTemp,Display,TEXT("EMBER_MESH_RENDERER_SUCCESS"));}
    if(FParse::Param(*Params,TEXT("VerifyTrailLength"))&&!VerifyTimeline(Asset,false,false,false,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyDistance"))&&!VerifyTimeline(Asset,true,true,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyStretch"))&&!VerifyTimeline(Asset,true,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyOrbit"))&&!VerifyTimeline(Asset,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyTimeline"))&&!VerifyTimeline(Asset))return 1;
    UE_LOG(LogTemp,Display,TEXT("EMBER_IMPORT_SUCCESS %s"),*Asset);
    return 0;
}
