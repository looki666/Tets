#include "GOAPGoal_InvestigateDisturbance.h"
#include "GOAPCore.h"
#include "SpiderAIAgent.h"

UGOAPGoal_InvestigateDisturbance::UGOAPGoal_InvestigateDisturbance()
{
    GoalName = "InvestigateDisturbance";
    Priority = 80.f; // High priority

    // The desired state is that there are no disturbances
    DesiredState.Add(FName("bHasDisturbance"), false);
}

bool UGOAPGoal_InvestigateDisturbance::IsGoalSatisfied()
{
    if (!OwnerAgent) return true; // Cannot be satisfied if there's no owner

    const bool* bHasDisturbance = OwnerAgent->CurrentWorldState.States.Find(FName("bHasDisturbance"));
    return bHasDisturbance && !(*bHasDisturbance);
}

float UGOAPGoal_InvestigateDisturbance::GetPriority()
{
    if (!OwnerAgent) return 0.f;

    const bool* bHasDisturbance = OwnerAgent->CurrentWorldState.States.Find(FName("bHasDisturbance"));
    if (bHasDisturbance && *bHasDisturbance)
    {
        return Priority;
    }

    return 0.f; // No priority if there's no disturbance
}
