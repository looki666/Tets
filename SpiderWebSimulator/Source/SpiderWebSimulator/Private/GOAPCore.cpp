#include "GOAPCore.h"
#include "SpiderAIAgent.h"

// --- UGOAPAction ---

UGOAPAction::UGOAPAction()
{
    Cost = 1.0f;
    Agent = nullptr;
}

void UGOAPAction::OnActionCreated_Implementation(ASpiderAIAgent* ActionAgent)
{
    Agent = ActionAgent;
}

bool UGOAPAction::IsActionDone_Implementation()
{
    // Base implementation should be overridden in subclasses
    return true;
}

bool UGOAPAction::PerformAction_Implementation()
{
    // Base implementation should be overridden in subclasses
    return true;
}

void UGOAPAction::ResetAction_Implementation()
{
    // Base implementation should be overridden in subclasses
}


// --- UGOAPGoal ---

float UGOAPGoal::GetPriority_Implementation()
{
    return Priority;
}
