#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPAction_InvestigateDisturbance.generated.h"

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPAction_InvestigateDisturbance : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPAction_InvestigateDisturbance();

    virtual bool IsActionPossible() override;
    virtual bool PerformAction() override;
    virtual bool IsActionDone() override;
    virtual void ResetAction() override;

private:
    TArray<int32> Path;
    int32 CurrentPathIndex;
    bool bIsAtDestination;
};
