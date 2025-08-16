#include "GOAPAction_BuildWeb.h"
#include "SpiderAIAgent.h"
#include "WebGenerator.h"
#include "WebPhysicsComponent.h"
#include "GOAPCore.h"

UGOAPAction_BuildWeb::UGOAPAction_BuildWeb()
{
    ActionName = "BuildWeb";
    Cost = 10.f;
    bWebBuilt = false;

    // This action can be performed if the spider does not have a web
    Preconditions.Add(FName("bHasWeb"), false);
    // After this action, the spider will have a web
    Effects.Add(FName("bHasWeb"), true);
}

bool UGOAPAction_BuildWeb::IsActionPossible()
{
    // For now, we assume it's always possible if the preconditions are met.
    // A more complex check could involve checking for nearby anchor points, etc.
    return true;
}

bool UGOAPAction_BuildWeb::PerformAction()
{
    if (!OwnerAgent || !OwnerAgent->WebGenerator || !OwnerAgent->PhysicsComponent)
    {
        return false;
    }

    if (!bWebBuilt)
    {
        TArray<FWebParticle> Particles;
        TArray<FWebConstraint> Constraints;

        // Use the agent's generator to create the web data
        OwnerAgent->WebGenerator->GenerateWeb(Particles, Constraints);

        // Initialize the physics component with this data
        OwnerAgent->PhysicsComponent->Initialize(MoveTemp(Particles), MoveTemp(Constraints));

        // Update the agent's world state to reflect that it now has a web
        OwnerAgent->UpdateWorldState(FName("bHasWeb"), true);

        bWebBuilt = true;
    }

    return true;
}

void UGOAPAction_BuildWeb::ResetAction()
{
    bWebBuilt = false;
}
