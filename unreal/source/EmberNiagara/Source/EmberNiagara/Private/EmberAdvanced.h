#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
class UNiagaraEmitter;
class UNiagaraSystem;
bool EmberAddLayer(UNiagaraSystem* System,const TSharedPtr<FJsonObject>& Effect,const FString& Name,FString& Error);
