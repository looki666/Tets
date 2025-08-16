#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPGoal_SatisfyHunger.generated.h"

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPGoal_SatisfyHunger : public UGOAPGoal
{
    GENERATED_BODY()

public:
    UGOAPGoal_SatisfyHunger();

    virtual bool IsGoalSatisfied() override;
    virtual float GetPriority() override;
};
