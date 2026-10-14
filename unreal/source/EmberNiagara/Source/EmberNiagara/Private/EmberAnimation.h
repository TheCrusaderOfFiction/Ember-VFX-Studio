#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
bool EmberValidateAnimation(const TSharedPtr<FJsonObject>& Effect,FString& Error);
FString EmberAnimatedValue(const TSharedPtr<FJsonObject>& Effect,const FString& Property,const FString& Base);
