#include "GOAPAction_InvestigateDisturbance.h"
#include "SpiderAIAgent.h"
#include "WebPhysicsComponent.h"
#include "WebPathfinder.h"
#include "SpiderSimTypes.h"

UGOAPAction_InvestigateDisturbance::UGOAPAction_InvestigateDisturbance()
{
    ActionName = "InvestigateDisturbance";
    Cost = 20.f; // High cost, as it's a reactive behavior
    CurrentPathIndex = 0;
    bIsAtDestination = false;

    Preconditions.Add(FName("bHasDisturbance"), true);
    Effects.Add(FName("bHasDisturbance"), false);
}

bool UGOAPAction_InvestigateDisturbance::IsActionPossible()
{
    if (!OwnerAgent || !OwnerAgent->PhysicsComponent || OwnerAgent->VibrationEvents.Num() == 0)
    {
        return false;
    }

    // Pathfind to the first reported disturbance
    FVector TargetLocation = OwnerAgent->VibrationEvents[0];

    int32 StartParticle = OwnerAgent->FindClosestParticleToLocation(OwnerAgent->GetActorLocation());
    int32 EndParticle = OwnerAgent->FindClosestParticleToLocation(TargetLocation);

    if (StartParticle == -1 || EndParticle == -1)
    {
        return false;
    }

    FWebPathfinder Pathfinder(&(OwnerAgent->PhysicsComponent->Particles), &(OwnerAgent->PhysicsComponent->Constraints));
    return Pathfinder.FindPath(StartParticle, EndParticle, Path);
}

bool UGOAPAction_InvestigateDisturbance::PerformAction()
{
    if (!OwnerAgent || Path.Num() == 0)
    {
        return false;
    }

    if (IsActionDone())
    {
        // Once we are at the destination, the disturbance is "investigated"
        OwnerAgent->UpdateWorldState(FName("bHasDisturbance"), false);
        OwnerAgent->VibrationEvents.Empty(); // Clear the events
        return true;
    }

    float MoveSpeed = 200.f; // Move faster when investigating
    float DeltaTime = OwnerAgent->GetWorld()->GetDeltaSeconds();
    FVector CurrentPosition = OwnerAgent->GetActorLocation();
    FVector TargetParticlePosition = OwnerAgent->PhysicsComponent->Particles[Path[CurrentPathIndex]].Position;

    FVector Direction = (TargetParticlePosition - CurrentPosition).GetSafeNormal();
    FVector NewPosition = CurrentPosition + Direction * MoveSpeed * DeltaTime;

    OwnerAgent->SetActorLocation(NewPosition);

    if (FVector::DistSquared(NewPosition, TargetParticlePosition) < FMath::Square(10.f))
    {
        CurrentPathIndex++;
        if (CurrentPathIndex >= Path.Num())
        {
            bIsAtDestination = true;
        }
    }

    return true;
}

bool UGOAPAction_InvestigateDisturbance::IsActionDone()
{
    return bIsAtDestination;
}

void UGOAPAction_InvestigateDisturbance::ResetAction()
{
    Path.Empty();
    CurrentPathIndex = 0;
    bIsAtDestination = false;
}
