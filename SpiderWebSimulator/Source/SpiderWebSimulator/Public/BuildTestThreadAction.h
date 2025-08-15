#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "BuildTestThreadAction.generated.h"

UCLASS()
class SPIDERWEBSIMULATOR_API UBuildTestThreadAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UBuildTestThreadAction();

    virtual bool IsActionDone_Implementation() override;
    virtual bool PerformAction_Implementation() override;
    virtual void ResetAction_Implementation() override;

private:
    bool bPerformed;
};
