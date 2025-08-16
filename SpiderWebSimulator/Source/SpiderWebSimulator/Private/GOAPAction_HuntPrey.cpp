#include "GOAPAction_HuntPrey.h"
#include "SpiderAIAgent.h"
#include "WebPhysicsComponent.h"
#include "WebPathfinder.h"
#include "SpiderSimTypes.h"
#include "FlyAgent.h"

UGOAPAction_HuntPrey::UGOAPAction_HuntPrey()
{
    ActionName = "HuntPrey";
    Cost = 15.f;
    CurrentPathIndex = 0;
    bIsAtDestination = false;
    TargetPrey = nullptr;

    Preconditions.Add(FName("bIsHungry"), true);
    Preconditions.Add(FName("bHasPreyOnWeb"), true);
    Effects.Add(FName("bIsHungry"), false);
}

bool UGOAPAction_HuntPrey::IsActionPossible()
{
    if (!OwnerAgent || !OwnerAgent->PhysicsComponent) return false;

    // Find the closest stuck fly
    float MinDistSq = TNumericLimits<float>::Max();
    AFlyAgent* ClosestFly = nullptr;

    for (auto const& [Fly, ParticleIndex] : OwnerAgent->PhysicsComponent->StuckFlies)
    {
        if (Fly)
        {
            float DistSq = FVector::DistSquared(OwnerAgent->GetActorLocation(), Fly->GetActorLocation());
            if (DistSq < MinDistSq)
            {
                MinDistSq = DistSq;
                ClosestFly = Fly;
            }
        }
    }

    if (!ClosestFly)
    {
        return false;
    }

    TargetPrey = ClosestFly;

    // Pathfind to the prey
    int32 StartParticle = OwnerAgent->FindClosestParticleToLocation(OwnerAgent->GetActorLocation());
    int32 EndParticle = OwnerAgent->FindClosestParticleToLocation(TargetPrey->GetActorLocation());

    if (StartParticle == -1 || EndParticle == -1)
    {
        return false;
    }

    FWebPathfinder Pathfinder(&(OwnerAgent->PhysicsComponent->Particles), &(OwnerAgent->PhysicsComponent->Constraints));
    return Pathfinder.FindPath(StartParticle, EndParticle, Path);
}

bool UGOAPAction_HuntPrey::PerformAction()
{
    if (!OwnerAgent || Path.Num() == 0 || !TargetPrey)
    {
        return false;
    }

    if (IsActionDone())
    {
        // Consume prey
        OwnerAgent->Hunger = 100.f; // Reset hunger
        OwnerAgent->PhysicsComponent->RemoveStuckFly(TargetPrey);
        TargetPrey->Destroy();
        return true;
    }

    // Move along path
    float MoveSpeed = 150.f;
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

bool UGOAPAction_HuntPrey::IsActionDone()
{
    return bIsAtDestination;
}

void UGOAPAction_HuntPrey::ResetAction()
{
    Path.Empty();
    CurrentPathIndex = 0;
    bIsAtDestination = false;
    TargetPrey = nullptr;
}
