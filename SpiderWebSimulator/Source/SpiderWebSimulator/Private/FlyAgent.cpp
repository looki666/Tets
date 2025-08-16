#include "FlyAgent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AFlyAgent::AFlyAgent()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Default movement bounds
    MovementBounds = FBox(FVector(-1000.f), FVector(1000.f));
}

void AFlyAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsStuck)
    {
        UpdateMovement(DeltaTime);
    }
}

void AFlyAgent::UpdateMovement(float DeltaTime)
{
    // Simple wander behavior
    FVector WanderForce = FMath::VRand() * WanderStrength;
    CurrentVelocity += WanderForce;
    CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(MovementSpeed);

    FVector NewPosition = GetActorLocation() + CurrentVelocity * DeltaTime;

    // Keep the agent within the bounds
    NewPosition = MovementBounds.GetClosestPointTo(NewPosition);

    // If the agent hits the bounds, reverse velocity slightly
    if (!MovementBounds.IsInside(GetActorLocation() + CurrentVelocity * DeltaTime * 0.1f))
    {
        CurrentVelocity = -CurrentVelocity * 0.5f;
    }

    SetActorLocation(NewPosition);
    SetActorRotation(CurrentVelocity.Rotation());
}
