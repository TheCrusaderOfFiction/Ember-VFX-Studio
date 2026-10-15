#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
bool EmberValidateLifetime(const TSharedPtr<FJsonObject>& Effect,FString& Error);
FString EmberLifetimeValue(const TSharedPtr<FJsonObject>& Effect,const TCHAR* Property,const FString& Fallback);
