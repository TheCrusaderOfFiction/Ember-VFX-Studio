#pragma once
#include "CoreMinimal.h"
class FEmberImporter {
public:
    static bool Import(const FString& Filename, FString& AssetPath, FString& Error);
};
