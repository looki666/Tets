#include "GOAPAction_RepairWeb.h"
#include "SpiderAIAgent.h"
#include "WebPhysicsComponent.h"
#include "SpiderSimTypes.h"

UGOAPAction_RepairWeb::UGOAPAction_RepairWeb()
{
    ActionName = "RepairWeb";
    Cost = 5.f;
    bRepairComplete = false;
    TargetConstraintIndex = -1;

    Preconditions.Add(FName("bWebIsDamaged"), true);
    Effects.Add(FName("bWebIsDamaged"), false);
}

bool UGOAPAction_RepairWeb::IsActionPossible()
{
    if (!OwnerAgent || !OwnerAgent->PhysicsComponent)
    {
        return false;
    }

    // Find the first broken constraint
    for (int32 i = 0; i < OwnerAgent->PhysicsComponent->Constraints.Num(); ++i)
    {
        if (OwnerAgent->PhysicsComponent->Constraints[i].bIsBroken)
        {
            TargetConstraintIndex = i;
            return true;
        }
    }

    return false;
}

bool UGOAPAction_RepairWeb::PerformAction()
{
    if (!OwnerAgent || !OwnerAgent->PhysicsComponent || TargetConstraintIndex == -1)
    {
        return false;
    }

    // In a full implementation, the agent would navigate to the broken constraint.
    // For now, we just repair it instantly.
    OwnerAgent->PhysicsComponent->Constraints[TargetConstraintIndex].bIsBroken = false;
    OwnerAgent->PhysicsComponent->Constraints[TargetConstraintIndex].Stress = 0.f;

    bRepairComplete = true;

    // After repairing one, check if there are any more broken ones.
    // If not, update the world state.
    bool bAnyBroken = false;
    for (const auto& Constraint : OwnerAgent->PhysicsComponent->Constraints)
    {
        if (Constraint.bIsBroken)
        {
            bAnyBroken = true;
            break;
        }
    }

    if (!bAnyBroken)
    {
        OwnerAgent->UpdateWorldState(FName("bWebIsDamaged"), false);
    }

    return true;
}

bool UGOAPAction_RepairWeb::IsActionDone()
{
    return bRepairComplete;
}

void UGOAPAction_RepairWeb::ResetAction()
{
    bRepairComplete = false;
    TargetConstraintIndex = -1;
}
