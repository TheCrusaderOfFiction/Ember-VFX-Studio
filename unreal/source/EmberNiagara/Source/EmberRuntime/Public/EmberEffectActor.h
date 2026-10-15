#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmberEffectActor.generated.h"
class UNiagaraSystem;
class UNiagaraComponent;
UCLASS(Blueprintable)
class EMBERRUNTIME_API AEmberEffectActor : public AActor {
 GENERATED_BODY()
public:
 AEmberEffectActor();
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ember") TObjectPtr<UNiagaraSystem> Effect;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ember") TObjectPtr<AActor> TargetActor;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ember") FString TargetActorName;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ember") FName Socket;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ember") TObjectPtr<UNiagaraComponent> Particles;
 virtual void OnConstruction(const FTransform& Transform) override;
 virtual void BeginPlay() override;
private:
 void Connect();
};
