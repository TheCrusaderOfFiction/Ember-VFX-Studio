#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
bool EmberSceneWorkspace(const TSharedPtr<FJsonObject>& Request,TSharedPtr<FJsonObject>& Result,FString& Error);
class UNiagaraSystem;
bool EmberCreateSceneActor(UNiagaraSystem* System,const TSharedPtr<FJsonObject>& Effect,FString& Error);
