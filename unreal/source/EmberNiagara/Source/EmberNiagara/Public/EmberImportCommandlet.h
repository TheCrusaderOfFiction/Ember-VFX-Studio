#pragma once
#include "Commandlets/Commandlet.h"
#include "EmberImportCommandlet.generated.h"
UCLASS()
class UEmberImportCommandlet : public UCommandlet {
    GENERATED_BODY()
public:
    UEmberImportCommandlet();
    virtual int32 Main(const FString& Params) override;
};
