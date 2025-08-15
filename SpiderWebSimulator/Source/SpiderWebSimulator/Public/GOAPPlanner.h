#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPCore.h"
#include "GOAPPlanner.generated.h"

class UGOAPAction;
class UGOAPGoal;
class ASpiderAIAgent;

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPPlanner : public UObject
{
    GENERATED_BODY()

public:
    TQueue<UGOAPAction*> Plan(ASpiderAIAgent* Agent, const TSet<UGOAPAction*>& AvailableActions, const FWorldState& WorldState, const FWorldState& GoalState);

private:
    // Helper struct for the A* algorithm
    struct FGOAPNode
    {
        FWorldState State;
        float G; // Cost from start to this node
        float H; // Heuristic cost from this node to goal
        UGOAPAction* Action; // Action that led to this state
        FGOAPNode* Parent;

        FGOAPNode(FWorldState InState, float InG, float InH, UGOAPAction* InAction, FGOAPNode* InParent)
            : State(InState), G(InG), H(InH), Action(InAction), Parent(InParent) {}

        float GetF() const { return G + H; }
    };

    float CalculateHeuristic(const FWorldState& From, const FWorldState& To) const;
    TQueue<UGOAPAction*> ReconstructPlan(FGOAPNode* GoalNode);
};
