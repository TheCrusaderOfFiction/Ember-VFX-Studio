#pragma once
#include "CoreMinimal.h"
class UMaterial;
class UNiagaraSystem;
UMaterial* EmberCreateSpriteMaterial(UNiagaraSystem* System,const FString& LayerName,const FString& Shape,bool Glow,FString& Error,bool Anime=false,int32 Steps=3,float Outline=.08f);
