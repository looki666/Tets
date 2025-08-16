#include "GOAPGoal_SatisfyHunger.h"
#include "GOAPCore.h"
#include "SpiderAIAgent.h"

UGOAPGoal_SatisfyHunger::UGOAPGoal_SatisfyHunger()
{
    GoalName = "SatisfyHunger";
    Priority = 70.f; // High priority, but maybe less than investigating a direct threat

    // The desired state is to not be hungry
    DesiredState.Add(FName("bIsHungry"), false);
}

bool UGOAPGoal_SatisfyHunger::IsGoalSatisfied()
{
    if (!OwnerAgent) return true;

    const bool* bIsHungry = OwnerAgent->CurrentWorldState.States.Find(FName("bIsHungry"));
    return bIsHungry && !(*bIsHungry);
}

float UGOAPGoal_SatisfyHunger::GetPriority()
{
    if (!OwnerAgent) return 0.f;

    const bool* bIsHungry = OwnerAgent->CurrentWorldState.States.Find(FName("bIsHungry"));
    if (bIsHungry && *bIsHungry)
    {
        return Priority;
    }

    return 0.f; // No priority if not hungry
}
