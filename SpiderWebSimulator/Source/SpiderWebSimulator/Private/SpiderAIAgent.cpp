#include "SpiderAIAgent.h"
#include "WebbingComponent.h"
#include "WebPhysicsComponent.h"
#include "WebGenerator.h"
#include "GOAPPlanner.h"
#include "GOAPCore.h"
#include "SpiderSimConfig.h"
#include "SpiderSimTypes.h"
#include "FlyAgent.h"

ASpiderAIAgent::ASpiderAIAgent()
{
    PrimaryActorTick.bCanEverTick = true;

    PhysicsComponent = CreateDefaultSubobject<UWebPhysicsComponent>(TEXT("PhysicsComponent"));
    WebbingComponent = CreateDefaultSubobject<UWebbingComponent>(TEXT("WebbingComponent"));
    WebGenerator = CreateDefaultSubobject<UWebGenerator>(TEXT("WebGenerator"));
    Planner = CreateDefaultSubobject<UGOAPPlanner>(TEXT("GOAPPlanner"));

    WebbingComponent->PhysicsComponent = PhysicsComponent;

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

    // Update hunger
    Hunger = FMath::Max(0.f, Hunger - HungerDepletionRate * DeltaTime);
    UpdateWorldState(FName("bIsHungry"), Hunger < HungerThreshold);


    // Update world state based on the environment
    if (PhysicsComponent)
    {
        if (PhysicsComponent->Constraints.Num() > 0)
        {
            bool bAnyBroken = false;
            for (const auto& Constraint : PhysicsComponent->Constraints)
            {
                if (Constraint.bIsBroken)
                {
                    bAnyBroken = true;
                    break;
                }
            }
            UpdateWorldState(FName("bWebIsDamaged"), bAnyBroken);

            // Check for new vibrations
            TArray<FVector> NewVibrations = PhysicsComponent->GetAndClearVibrationEvents();
            if (NewVibrations.Num() > 0)
            {
                VibrationEvents.Append(NewVibrations);
                UpdateWorldState(FName("bHasDisturbance"), true);
            }
        }

        // Check for prey on the web
        UpdateWorldState(FName("bHasPreyOnWeb"), PhysicsComponent->StuckFlies.Num() > 0);
    }

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

int32 ASpiderAIAgent::FindClosestParticleToLocation(const FVector& Location) const
{
    int32 ClosestParticleIndex = -1;
    float MinDistSq = TNumericLimits<float>::Max();

    if (!PhysicsComponent) return -1;

    for (int32 i = 0; i < PhysicsComponent->Particles.Num(); ++i)
    {
        float DistSq = FVector::DistSquared(Location, PhysicsComponent->Particles[i].Position);
        if (DistSq < MinDistSq)
        {
            MinDistSq = DistSq;
            ClosestParticleIndex = i;
        }
    }
    return ClosestParticleIndex;
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
