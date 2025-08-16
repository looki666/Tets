#include "GOAPAction_MoveToLocation.h"
#include "SpiderAIAgent.h"
#include "WebPhysicsComponent.h"
#include "WebPathfinder.h"
#include "SpiderSimTypes.h"

UGOAPAction_MoveToLocation::UGOAPAction_MoveToLocation()
{
    ActionName = "MoveToLocation";
    Cost = 1.f; // Cost will be distance-dependent, calculated in IsActionPossible
    CurrentPathIndex = 0;
    TargetLocation = FVector::ZeroVector;
}

void UGOAPAction_MoveToLocation::OnActionCreated(ASpiderAIAgent* NewOwner)
{
    Super::OnActionCreated(NewOwner);
}

void UGOAPAction_MoveToLocation::SetTargetLocation(const FVector& InTarget)
{
    TargetLocation = InTarget;
}

bool UGOAPAction_MoveToLocation::IsActionPossible()
{
    if (!OwnerAgent || !OwnerAgent->PhysicsComponent || TargetLocation.IsZero())
    {
        return false;
    }

    int32 StartParticle = OwnerAgent->FindClosestParticleToLocation(OwnerAgent->GetActorLocation());
    int32 EndParticle = OwnerAgent->FindClosestParticleToLocation(TargetLocation);

    if (StartParticle == -1 || EndParticle == -1 || StartParticle == EndParticle)
    {
        return false;
    }

    FWebPathfinder Pathfinder(&(OwnerAgent->PhysicsComponent->Particles), &(OwnerAgent->PhysicsComponent->Constraints));
    bool bPathFound = Pathfinder.FindPath(StartParticle, EndParticle, Path);

    if (bPathFound)
    {
        // Dynamically adjust the cost of this action based on the path length
        float PathLength = 0.f;
        for (int32 i = 0; i < Path.Num() - 1; ++i)
        {
            PathLength += FVector::Dist(OwnerAgent->PhysicsComponent->Particles[Path[i]].Position, OwnerAgent->PhysicsComponent->Particles[Path[i+1]].Position);
        }
        Cost = PathLength / 100.f; // Arbitrary scaling factor
    }

    return bPathFound;
}

bool UGOAPAction_MoveToLocation::PerformAction()
{
    if (!OwnerAgent || Path.Num() == 0)
    {
        return false;
    }

    if (IsActionDone())
    {
        return true;
    }

    float MoveSpeed = 150.f; // Units per second
    float DeltaTime = OwnerAgent->GetWorld()->GetDeltaSeconds();
    FVector CurrentPosition = OwnerAgent->GetActorLocation();
    FVector TargetParticlePosition = OwnerAgent->PhysicsComponent->Particles[Path[CurrentPathIndex]].Position;

    FVector Direction = (TargetParticlePosition - CurrentPosition).GetSafeNormal();
    FVector NewPosition = CurrentPosition + Direction * MoveSpeed * DeltaTime;

    OwnerAgent->SetActorLocation(NewPosition);

    // Check if we are close enough to the current target particle
    if (FVector::DistSquared(NewPosition, TargetParticlePosition) < FMath::Square(10.f))
    {
        CurrentPathIndex++;
    }

    return true;
}

bool UGOAPAction_MoveToLocation::IsActionDone()
{
    return CurrentPathIndex >= Path.Num();
}

void UGOAPAction_MoveToLocation::ResetAction()
{
    Path.Empty();
    CurrentPathIndex = 0;
    TargetLocation = FVector::ZeroVector;
}
