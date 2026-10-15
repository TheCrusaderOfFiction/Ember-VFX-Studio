#include "EmberEffectActor.h"
#include "NiagaraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "EngineUtils.h"
AEmberEffectActor::AEmberEffectActor(){RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));Particles=CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));Particles->SetupAttachment(RootComponent);}
void AEmberEffectActor::Connect(){
 if(Effect&&Particles->GetAsset()!=Effect)Particles->SetAsset(Effect);
 AActor* Target=TargetActor;if(!Target&&!TargetActorName.IsEmpty()&&GetWorld())for(TActorIterator<AActor> It(GetWorld());It;++It)if(It->GetName()==TargetActorName||It->ActorHasTag(FName(*TargetActorName))){Target=*It;break;}
 if(!Target||Target==this)return;USceneComponent* Parent=Target->GetRootComponent();TInlineComponentArray<USkinnedMeshComponent*> Meshes(Target);if(Meshes.Num())Parent=Meshes[0];if(!Parent)return;
 Particles->AttachToComponent(Parent,FAttachmentTransformRules::SnapToTargetNotIncludingScale,Socket);
}
void AEmberEffectActor::OnConstruction(const FTransform& Transform){Super::OnConstruction(Transform);Connect();}
void AEmberEffectActor::BeginPlay(){Super::BeginPlay();Connect();Particles->Activate(true);}
