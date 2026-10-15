#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
class UNiagaraEmitter;
class UNiagaraScript;
bool EmberValidateEvents(const TSharedPtr<FJsonObject>& Effect,FString& Error);
TSharedPtr<FJsonObject> EmberResponseEffect(const TSharedPtr<FJsonObject>& Rule,const TSharedPtr<FJsonObject>& Parent);
UNiagaraScript* EmberEventModule(UNiagaraEmitter* Emitter,FName Event,bool Receive,double CenterZ=8);
bool EmberReceiveEvents(UNiagaraEmitter* Emitter,const FGuid& Source,FName Event,int32 Count,double CenterZ);
