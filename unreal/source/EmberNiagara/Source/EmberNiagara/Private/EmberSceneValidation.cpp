#include "EmberSceneValidation.h"
#include "EmberImporter.h"
#include "EmberScene.h"
#include "EmberEffectActor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/Blueprint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "FXSystem.h"
#include "RendererInterface.h"
#include "NiagaraSystem.h"
#include "NiagaraScript.h"
#include "NiagaraShared.h"
#include "Misc/FileHelper.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraEmitterInstance.h"
#include "NiagaraDataSetAccessor.h"
#include "NiagaraParameterStore.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
namespace {
bool Save(UObject* Asset){FAssetRegistryModule::AssetCreated(Asset);auto* Package=Asset->GetOutermost();Package->MarkPackageDirty();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(Package,Asset,*FPackageName::LongPackageNameToFilename(Package->GetName(),FPackageName::GetAssetPackageExtension()),Args);}
}
bool EmberVerifySceneSuite(const FString& Folder,FString& Error){
 TGuardValue<bool> ClientGuard(GIsClient,true);
 auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));auto* Skeletal=LoadObject<USkeletalMesh>(nullptr,TEXT("/Engine/EngineMeshes/SkeletalCube.SkeletalCube"));if(!Cube||!Skeletal){Error=TEXT("Missing engine validation meshes");return false;}
 auto* StaticCopy=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/EmberQA/StaticCube.StaticCube"));if(!StaticCopy){StaticCopy=DuplicateObject<UStaticMesh>(Cube,CreatePackage(TEXT("/Game/EmberQA/StaticCube")),TEXT("StaticCube"));StaticCopy->bAllowCPUAccess=true;if(!Save(StaticCopy))return false;}auto* SkeletalCopy=LoadObject<USkeletalMesh>(nullptr,TEXT("/Game/EmberQA/SkeletalCube.SkeletalCube"));if(!SkeletalCopy){SkeletalCopy=DuplicateObject<USkeletalMesh>(Skeletal,CreatePackage(TEXT("/Game/EmberQA/SkeletalCube")),TEXT("SkeletalCube"));if(!Save(SkeletalCopy))return false;}
 for(const FString Kind:{TEXT("static"),TEXT("skeletal")}){auto Request=MakeShared<FJsonObject>();Request->SetStringField(TEXT("action"),TEXT("surface"));Request->SetStringField(TEXT("asset"),Kind==TEXT("static")?TEXT("/Game/EmberQA/StaticCube.StaticCube"):TEXT("/Game/EmberQA/SkeletalCube.SkeletalCube"));TSharedPtr<FJsonObject> Result=MakeShared<FJsonObject>();if(!EmberSceneWorkspace(Request,Result,Error)||Result->GetArrayField(TEXT("points")).Num()!=2048)return false;UE_LOG(LogTemp,Display,TEXT("EMBER_SURFACE_POINTS %s 2048"),*Kind);}
 bool Valid=true;
 for(const FString Kind:{TEXT("controls"),TEXT("flow"),TEXT("static"),TEXT("skeletal"),TEXT("custom"),TEXT("gpu"),TEXT("gpu-scene"),TEXT("gpu-kill"),TEXT("gpu-static"),TEXT("gpu-skeletal"),TEXT("scene"),TEXT("scene-bounce")}){
  FString Asset;if(!FEmberImporter::Import(FPaths::Combine(Folder,Kind+TEXT(".json")),Asset,Error))return false;auto* System=LoadObject<UNiagaraSystem>(nullptr,*Asset);if(!System)return false;
  if(Kind==TEXT("controls")||Kind.StartsWith(TEXT("scene"))){const auto* D=System->GetEmitterHandles()[0].GetEmitterData();FFileHelper::SaveStringToFile(D->SpawnScriptProps.Script->GetVMExecutableData().LastHlslTranslation,*FPaths::Combine(Folder,Kind+TEXT("-spawn.hlsl")));FFileHelper::SaveStringToFile(D->UpdateScriptProps.Script->GetVMExecutableData().LastHlslTranslation,*FPaths::Combine(Folder,Kind+TEXT("-update.hlsl")));}
  if(Kind.StartsWith(TEXT("gpu"))){const auto* Data=System->GetEmitterHandles()[0].GetEmitterData();Valid&=Data->SimTarget==ENiagaraSimTarget::GPUComputeSim&&Data->CalculateBoundsMode==ENiagaraEmitterCalculateBoundMode::Fixed;auto* Resource=const_cast<UNiagaraScript*>(Data->GetGPUComputeScript())->GetRenderThreadScript();if(Resource){Resource->FinishCompilation();Valid&=Resource->GetGameThreadShaderMap()!=nullptr;if(!Valid)for(const FString& Message:Resource->GetCompileErrors())Error+=Message;}else Valid=false;UE_LOG(LogTemp,Display,TEXT("EMBER_GPU_COMPILED %d"),Valid);continue;}
  auto* World=UWorld::CreateWorld(EWorldType::Game,false);GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);if(!World->Scene)FModuleManager::LoadModuleChecked<IRendererModule>(TEXT("Renderer")).AllocateScene(World,false,false,World->GetFeatureLevel());if(!World->GetPhysicsScene())World->CreatePhysicsScene();World->bEnableTraceCollision=true;World->FXSystem=FFXSystemInterface::Create(World->GetFeatureLevel(),World->Scene);
  auto* Floor=World->SpawnActor<AStaticMeshActor>();Floor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);Floor->GetStaticMeshComponent()->SetStaticMesh(Cube);Floor->SetActorLocation(FVector(0,0,-50));Floor->SetActorScale3D(FVector(100,100,1));Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);if(Kind.StartsWith(TEXT("scene"))){World->InitializeActorsForPlay(FURL());World->BeginPlay();Floor->GetStaticMeshComponent()->RecreatePhysicsState();for(int32 Warmup=0;Warmup<3;Warmup++){GFrameCounter++;World->Tick(LEVELTICK_All,1.f/60);}FHitResult Hit;const bool Trace=World->LineTraceSingleByChannel(Hit,FVector(0,0,100),FVector(0,0,-100),ECC_WorldStatic);UE_LOG(LogTemp,Display,TEXT("EMBER_WORLD_TRACE %d z=%f"),Trace,Hit.ImpactPoint.Z);}
  auto* Component=NewObject<UNiagaraComponent>(World);Component->SetAutoActivate(false);Component->SetForceSolo(true);Component->SetAsset(System);Component->RegisterComponentWithWorld(World);
  if(Kind==TEXT("skeletal")){FActorSpawnParameters Params;Params.Name=TEXT("QACharacter");auto* Character=World->SpawnActor<ASkeletalMeshActor>(Params);Character->GetSkeletalMeshComponent()->SetSkeletalMesh(SkeletalCopy);Character->SetActorLocation(FVector(200,0,0));Component->AttachToComponent(Character->GetSkeletalMeshComponent(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
   auto& Registry=FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();TArray<FAssetData> Blueprints;Registry.GetAssetsByPath(TEXT("/Game/Ember/Actors"),Blueprints,true);UBlueprint* BP=nullptr;for(const auto& A:Blueprints)if(A.AssetName.ToString().StartsWith(TEXT("BP_")+System->GetName()))BP=Cast<UBlueprint>(A.GetAsset());if(!BP){Valid=false;Error=TEXT("Companion actor not saved");}else{auto* Actor=World->SpawnActor<AEmberEffectActor>(BP->GeneratedClass);Valid&=Actor&&Actor->Particles->GetAttachParent()==Character->GetSkeletalMeshComponent();Character->SetActorLocation(FVector(250,0,0));Valid&=Actor&&FMath::Abs(Actor->Particles->GetComponentLocation().X-250)<1;}}
  Component->Activate(true);for(int32 Step=0;Step<(Kind.StartsWith(TEXT("scene"))?90:45);Step++){if(Kind.StartsWith(TEXT("scene"))){GFrameCounter++;World->Tick(LEVELTICK_All,1.f/60);FPlatformProcess::Sleep(.001f);}else Component->AdvanceSimulation(1,1.f/60);}
  auto Controller=Component->GetSystemInstanceController();auto* Instance=Controller.IsValid()?Controller->GetSystemInstance_Unsafe():nullptr;if(!Instance){Error=TEXT("No live system instance for ")+Kind;Valid=false;}else if(Kind==TEXT("custom")){Valid&=Instance->GetEmitters().Num()==2&&Instance->GetEmitters()[1]->GetNumParticles()==30;UE_LOG(LogTemp,Display,TEXT("EMBER_CUSTOM_RESPONSE_COUNT %d"),Instance->GetEmitters().Num()>1?Instance->GetEmitters()[1]->GetNumParticles():-1);}else{auto& Emitter=Instance->GetEmitters()[0].Get();const int32 Count=Emitter.GetNumParticles();UE_LOG(LogTemp,Display,TEXT("EMBER_SCENE_SAMPLE %s count=%d"),*Kind,Count);if(Kind.StartsWith(TEXT("scene"))){if(Count){const auto P=FNiagaraDataSetAccessor<FNiagaraPosition>(Emitter.GetParticleData(),TEXT("Position")).GetReader(Emitter.GetParticleData());if(P.IsValid())UE_LOG(LogTemp,Display,TEXT("EMBER_SCENE_Z %f"),P.Get(0).Z);}Valid&=Kind==TEXT("scene")?Count==0:Count==10;if(Instance->GetEmitters().Num()!=2||Instance->GetEmitters()[1]->GetNumParticles()!=30)Valid=false;UE_LOG(LogTemp,Display,TEXT("EMBER_SCENE_EVENT_COUNT %d"),Instance->GetEmitters().Num()>1?Instance->GetEmitters()[1]->GetNumParticles():-1);}else{Valid&=Kind==TEXT("flow")?(Count>=29&&Count<=30):Count==(Kind==TEXT("controls")?20:10);const auto& Data=Emitter.GetParticleData();const auto Pos=FNiagaraDataSetAccessor<FNiagaraPosition>(Data,Kind==TEXT("controls")?TEXT("EmberRenderedPosition"):TEXT("Position")).GetReader(Data);const auto Color=FNiagaraDataSetAccessor<FLinearColor>(Data,TEXT("Color")).GetReader(Data);if(!Pos.IsValid())Valid=false;else {float MaxRadius=0;for(int32 I=0;I<Count;I++){const auto P=Pos.Get(I);MaxRadius=FMath::Max(MaxRadius,FMath::Max3(FMath::Abs(P.X),FMath::Abs(P.Y),FMath::Abs(P.Z)));if(Kind==TEXT("controls")){if(I==0)UE_LOG(LogTemp,Display,TEXT("EMBER_CONTROLS_POSITION %f %f %f color %f %f %f scale %f"),P.X,P.Y,P.Z,Color.IsValid()?Color.Get(I).R:-1.f,Color.IsValid()?Color.Get(I).G:-1.f,Color.IsValid()?Color.Get(I).B:-1.f,System->GetExposedParameters().GetParameterValue<float>(FNiagaraVariable(FNiagaraTypeDefinition::GetFloatDef(),TEXT("User.EmberScale"))));if(P.X<195||P.X>205||!Color.IsValid()||Color.Get(I).R>.01||Color.Get(I).G<.9)Valid=false;}else if(FMath::Max3(FMath::Abs(P.X),FMath::Abs(P.Y),FMath::Abs(P.Z))>110)Valid=false;}if(Kind==TEXT("static")||Kind==TEXT("skeletal")){UE_LOG(LogTemp,Display,TEXT("EMBER_SURFACE_RADIUS %s %f"),*Kind,MaxRadius);Valid&=MaxRadius>5;}}}}
  Component->DestroyComponent();GEngine->DestroyWorldContext(World);World->DestroyWorld(false);if(!Valid){Error=TEXT("Live scene suite failed at ")+Kind;return false;}
 }
 if(Valid)UE_LOG(LogTemp,Display,TEXT("EMBER_SCENE_SUITE_SUCCESS"));return Valid;
}
