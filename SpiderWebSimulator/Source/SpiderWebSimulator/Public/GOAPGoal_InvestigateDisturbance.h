#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPGoal_InvestigateDisturbance.generated.h"

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPGoal_InvestigateDisturbance : public UGOAPGoal
{
    GENERATED_BODY()

public:
    UGOAPGoal_InvestigateDisturbance();

    virtual bool IsGoalSatisfied() override;
    virtual float GetPriority() override;
};
