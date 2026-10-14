#include "EmberImportCommandlet.h"
#include "EmberImporter.h"
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
            for(int32 EmitterIndex=0;EmitterIndex<3;EmitterIndex++){auto& Emitter=Instance->GetEmitters()[EmitterIndex].Get();const auto& Data=Emitter.GetParticleData();const auto Sizes=FNiagaraDataSetAccessor<FVector2f>(Data,TEXT("SpriteSize")).GetReader(Data);const int32 TrailCount=Emitter.GetNumParticles();if(TrailCount!=100||!Sizes.IsValid()){Valid=false;continue;}const float Expected=EmitterIndex==0?(Step<=3?2.0f:10.0f):EmitterIndex==1?1.0f:2.0f;for(int32 Index=0;Index<TrailCount;Index++){const auto Size=Sizes.Get(Index);if(!FMath::IsFinite(Size.X)||!FMath::IsFinite(Size.Y)||Size.X<=0||((Step<=3||Step>=6)&&FMath::Abs(Size.Y/Size.X-Expected)>.01f))Valid=false;}UE_LOG(LogTemp,Display,TEXT("EMBER_TRAIL_SAMPLE %d %.2f ratio %.3f"),EmitterIndex,Step*.25,Sizes.Get(0).Y/Sizes.Get(0).X);}
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
    FString File, Asset, Error;
    if(!FParse::Value(*Params,TEXT("Effect="),File)){UE_LOG(LogTemp,Error,TEXT("Missing -Effect=<Ember JSON file>"));return 1;}
    if(!FEmberImporter::Import(File,Asset,Error)){UE_LOG(LogTemp,Error,TEXT("EMBER_IMPORT_FAILED %s"),*Error);return 1;}
    if(FParse::Param(*Params,TEXT("VerifyTrailLength"))&&!VerifyTimeline(Asset,false,false,false,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyDistance"))&&!VerifyTimeline(Asset,true,true,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyStretch"))&&!VerifyTimeline(Asset,true,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyOrbit"))&&!VerifyTimeline(Asset,true))return 1;
    if(FParse::Param(*Params,TEXT("VerifyTimeline"))&&!VerifyTimeline(Asset))return 1;
    UE_LOG(LogTemp,Display,TEXT("EMBER_IMPORT_SUCCESS %s"),*Asset);
    return 0;
}
