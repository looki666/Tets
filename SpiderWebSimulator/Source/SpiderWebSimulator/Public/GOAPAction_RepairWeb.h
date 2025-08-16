#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPAction_RepairWeb.generated.h"

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPAction_RepairWeb : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPAction_RepairWeb();

    virtual bool IsActionPossible() override;
    virtual bool PerformAction() override;
    virtual bool IsActionDone() override;
    virtual void ResetAction() override;

private:
    bool bRepairComplete;
    int32 TargetConstraintIndex;
};
