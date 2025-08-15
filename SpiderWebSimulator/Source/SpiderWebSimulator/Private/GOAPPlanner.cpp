#include "GOAPPlanner.h"
#include "GOAPCore.h"
#include "SpiderAIAgent.h"

TQueue<UGOAPAction*> UGOAPPlanner::Plan(ASpiderAIAgent* Agent, const TSet<UGOAPAction*>& AvailableActions, const FWorldState& WorldState, const FWorldState& GoalState)
{
    TArray<FGOAPNode*> OpenList;
    TArray<FGOAPNode*> ClosedList;
    TQueue<UGOAPAction*> Plan;

    FGOAPNode* StartNode = new FGOAPNode(WorldState, 0.f, CalculateHeuristic(WorldState, GoalState), nullptr, nullptr);
    OpenList.Add(StartNode);

    while (OpenList.Num() > 0)
    {
        // Find the node with the lowest F cost in the open list
        FGOAPNode* CurrentNode = OpenList[0];
        int32 CurrentIndex = 0;
        for (int32 i = 1; i < OpenList.Num(); ++i)
        {
            if (OpenList[i]->GetF() < CurrentNode->GetF())
            {
                CurrentNode = OpenList[i];
                CurrentIndex = i;
            }
        }

        // Move the current node from open to closed
        OpenList.RemoveAt(CurrentIndex);
        ClosedList.Add(CurrentNode);

        // Check if the goal is satisfied
        bool bGoalMet = true;
        for (const auto& GoalPair : GoalState.States)
        {
            if (!CurrentNode->State.States.Contains(GoalPair.Key) || CurrentNode->State.States[GoalPair.Key] != GoalPair.Value)
            {
                bGoalMet = false;
                break;
            }
        }

        if (bGoalMet)
        {
            return ReconstructPlan(CurrentNode);
        }

        // Expand the current node
        for (UGOAPAction* Action : AvailableActions)
        {
            // Check if preconditions are met
            bool bPreconditionsMet = true;
            for (const auto& PreconditionPair : Action->Preconditions.States)
            {
                if (!CurrentNode->State.States.Contains(PreconditionPair.Key) || CurrentNode->State.States[PreconditionPair.Key] != PreconditionPair.Value)
                {
                    bPreconditionsMet = false;
                    break;
                }
            }

            if (bPreconditionsMet)
            {
                FWorldState SuccessorState = CurrentNode->State;
                for (const auto& EffectPair : Action->Effects.States)
                {
                    SuccessorState.States.FindOrAdd(EffectPair.Key) = EffectPair.Value;
                }

                float SuccessorG = CurrentNode->G + Action->Cost;
                float SuccessorH = CalculateHeuristic(SuccessorState, GoalState);
                FGOAPNode* SuccessorNode = new FGOAPNode(SuccessorState, SuccessorG, SuccessorH, Action, CurrentNode);

                // Check if this node is already in the closed list
                bool bInClosedList = false;
                for (FGOAPNode* Node : ClosedList)
                {
                    if (Node->State == SuccessorNode->State)
                    {
                        bInClosedList = true;
                        break;
                    }
                }

                if (bInClosedList)
                {
                    delete SuccessorNode;
                    continue;
                }

                // Check if this node is already in the open list with a lower G cost
                bool bInOpenList = false;
                for (FGOAPNode* Node : OpenList)
                {
                    if (Node->State == SuccessorNode->State && Node->G <= SuccessorNode->G)
                    {
                        bInOpenList = true;
                        break;
                    }
                }

                if (!bInOpenList)
                {
                    OpenList.Add(SuccessorNode);
                }
                else
                {
                     delete SuccessorNode;
                }
            }
        }
    }

    // No plan found
    return Plan;
}

float UGOAPPlanner::CalculateHeuristic(const FWorldState& From, const FWorldState& To) const
{
    float Heuristic = 0.f;
    for (const auto& ToPair : To.States)
    {
        if (!From.States.Contains(ToPair.Key) || From.States[ToPair.Key] != ToPair.Value)
        {
            Heuristic++;
        }
    }
    return Heuristic;
}

TQueue<UGOAPAction*> UGOAPPlanner::ReconstructPlan(FGOAPNode* GoalNode)
{
    TQueue<UGOAPAction*> Plan;
    FGOAPNode* CurrentNode = GoalNode;
    while (CurrentNode != nullptr && CurrentNode->Action != nullptr)
    {
        Plan.Enqueue(CurrentNode->Action);
        CurrentNode = CurrentNode->Parent;
    }

    // The plan is reconstructed backwards, so we need to reverse it.
    // TQueue doesn't have a reverse, so we'll use a TArray as an intermediary.
    TArray<UGOAPAction*> TempArray;
    while (!Plan.IsEmpty())
    {
        UGOAPAction* Action;
        Plan.Dequeue(Action);
        TempArray.Add(Action);
    }
    Algo::Reverse(TempArray);
    for (UGOAPAction* Action : TempArray)
    {
        Plan.Enqueue(Action);
    }

    return Plan;
}
