#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
class UNiagaraSystem;class UMaterial;
bool EmberValidateFlipbook(const TSharedPtr<FJsonObject>& Effect,FString& Error);
UMaterial* EmberCreateFlipbookMaterial(UNiagaraSystem* System,const TSharedPtr<FJsonObject>& Effect,const FString& LayerName,FString& Error);
