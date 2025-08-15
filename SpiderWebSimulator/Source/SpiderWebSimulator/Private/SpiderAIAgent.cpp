#include "SpiderAIAgent.h"
#include "WebbingComponent.h"
#include "GOAPPlanner.h"
#include "GOAPCore.h"
#include "SpiderSimConfig.h"

ASpiderAIAgent::ASpiderAIAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    WebbingComponent = CreateDefaultSubobject<UWebbingComponent>(TEXT("WebbingComponent"));
    Planner = CreateDefaultSubobject<UGOAPPlanner>(TEXT("GOAPPlanner"));

    TimeSinceLastPlan = 0.f;
    CurrentGoal = nullptr;
    Config = nullptr;
}

void ASpiderAIAgent::BeginPlay()
{
    Super::BeginPlay();

    for (UGOAPAction* Action : AvailableActions)
    {
        if (Action)
        {
            Action->OnActionCreated(this);
        }
    }
}

void ASpiderAIAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateGOAP();
}

void ASpiderAIAgent::SetConfig(USpiderSimConfig* InConfig)
{
    Config = InConfig;
}

void ASpiderAIAgent::UpdateWorldState(FName Key, bool Value)
{
    CurrentWorldState.States.FindOrAdd(Key) = Value;
}

void ASpiderAIAgent::UpdateGOAP()
{
    TimeSinceLastPlan += GetWorld()->GetDeltaSeconds();

    if (CurrentPlan.Num() == 0 || (Config && TimeSinceLastPlan >= Config->GoalChangeFrequency))
    {
        FindNewPlan();
    }

    ExecuteCurrentAction();
}

void ASpiderAIAgent::FindNewPlan()
{
    TimeSinceLastPlan = 0.f;

    // Find the highest priority goal
    UGOAPGoal* BestGoal = nullptr;
    float MaxPriority = -1.f;

    for (UGOAPGoal* Goal : AvailableGoals)
    {
        if (Goal)
        {
            float Priority = Goal->GetPriority();
            if (Priority > MaxPriority)
            {
                MaxPriority = Priority;
                BestGoal = Goal;
            }
        }
    }

    if (BestGoal)
    {
        CurrentGoal = BestGoal;

        TQueue<UGOAPAction*> PlanQueue = Planner->Plan(this, AvailableActions, CurrentWorldState, CurrentGoal->DesiredState);

        CurrentPlan.Empty();
        while(!PlanQueue.IsEmpty())
        {
            UGOAPAction* Action;
            PlanQueue.Dequeue(Action);
            CurrentPlan.Add(Action);
        }
    }
}

void ASpiderAIAgent::ExecuteCurrentAction()
{
    if (CurrentPlan.Num() > 0)
    {
        UGOAPAction* CurrentAction = CurrentPlan[0];
        if (CurrentAction)
        {
            if (CurrentAction->IsActionDone())
            {
                CurrentPlan.RemoveAt(0);
                if (CurrentPlan.Num() > 0)
                {
                    CurrentPlan[0]->ResetAction();
                }
            }
            else
            {
                CurrentAction->PerformAction();
            }
        }
    }
}
