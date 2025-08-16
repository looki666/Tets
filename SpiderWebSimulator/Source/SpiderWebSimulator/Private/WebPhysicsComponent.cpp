#include "WebPhysicsComponent.h"
#include "SpiderSimTypes.h"
#include "FlyAgent.h"
#include "EngineUtils.h"

UWebPhysicsComponent::UWebPhysicsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWebPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Particles.Num() == 0)
    {
        return;
    }

    WindTime += DeltaTime;

    HandleFlyInteractions(DeltaTime);
    Integrate(DeltaTime);
    SolveConstraints();

    // Update stuck flies positions after the simulation step
    for (auto const& [Fly, ParticleIndex] : StuckFlies)
    {
        if (Fly && Particles.IsValidIndex(ParticleIndex))
        {
            Fly->SetActorLocation(Particles[ParticleIndex].Position);
        }
    }
}

void UWebPhysicsComponent::Initialize(TArray<FWebParticle>&& InParticles, TArray<FWebConstraint>&& InConstraints)
{
    Particles = MoveTemp(InParticles);
    Constraints = MoveTemp(InConstraints);
    LastFrameStress.Empty();
    VibrationEvents.Empty();
    StuckFlies.Empty();
}

void UWebPhysicsComponent::ClearWeb()
{
    Particles.Empty();
    Constraints.Empty();
    LastFrameStress.Empty();
    VibrationEvents.Empty();
    StuckFlies.Empty();
}

void UWebPhysicsComponent::RemoveStuckFly(AFlyAgent* FlyToRemove)
{
    if (!FlyToRemove || !StuckFlies.Contains(FlyToRemove))
    {
        return;
    }

    int32 FlyParticleIndex = StuckFlies[FlyToRemove];
    StuckFlies.Remove(FlyToRemove);

    if (Particles.IsValidIndex(FlyParticleIndex))
    {
        // Mark the particle as inactive instead of removing it to preserve indices.
        // A more robust system would have a particle pooling or defragmentation strategy.
        // For now, we pin it to a far-away location to effectively remove it from simulation.
        Particles[FlyParticleIndex].bIsPinned = true;
        Particles[FlyParticleIndex].Position = FVector(0,0, -100000);

        // Remove constraints attached to this particle
        Constraints.RemoveAll([&](const FWebConstraint& Constraint)
        {
            return Constraint.Particle1Index == FlyParticleIndex || Constraint.Particle2Index == FlyParticleIndex;
        });
    }
}


void UWebPhysicsComponent::Integrate(float DeltaTime)
{
    // This is a single step of Verlet integration
    for (auto& Particle : Particles)
    {
        if (!Particle.bIsPinned)
        {
            // Apply external forces
            Particle.ApplyForce(Gravity);

            // Apply wind force with turbulence
            if (WindStrength.SizeSquared() > 0.f)
            {
                FVector Turbulence = FVector(
                    FMath::PerlinNoise3D(Particle.Position * 0.01f + FVector(WindTime * 0.5f)),
                    FMath::PerlinNoise3D(Particle.Position * 0.01f + FVector(WindTime * 0.5f + 10.f)),
                    FMath::PerlinNoise3D(Particle.Position * 0.01f + FVector(WindTime * 0.5f + 20.f))
                ) * WindTurbulence;
                Particle.ApplyForce(WindStrength + Turbulence * WindStrength.Size());
            }

            const FVector TempPosition = Particle.Position;
            // Verlet integration formula
            Particle.Position += (Particle.Position - Particle.OldPosition) * Damping + Particle.Acceleration * DeltaTime * DeltaTime;
            Particle.OldPosition = TempPosition;

            // Reset acceleration for the next frame
            Particle.Acceleration = FVector::ZeroVector;
        }
    }
}

void UWebPhysicsComponent::HandleFlyInteractions(float DeltaTime)
{
    if (!GetWorld()) return;

    for (TActorIterator<AFlyAgent> It(GetWorld()); It; ++It)
    {
        AFlyAgent* Fly = *It;
        if (Fly && !Fly->bIsStuck)
        {
            FVector FlyLocation = Fly->GetActorLocation();
            float FlyRadius = Fly->CollisionSphere->GetScaledSphereRadius();

            for (const auto& Constraint : Constraints)
            {
                if (Constraint.bIsSticky && !Constraint.bIsBroken)
                {
                    const FVector P1 = Particles[Constraint.Particle1Index].Position;
                    const FVector P2 = Particles[Constraint.Particle2Index].Position;

                    FVector ClosestPoint = FMath::ClosestPointOnSegment(FlyLocation, P1, P2);
                    float DistSq = FVector::DistSquared(FlyLocation, ClosestPoint);

                    if (DistSq < FMath::Square(FlyRadius + 10.f)) // 10.f is a stickiness threshold
                    {
                        Fly->bIsStuck = true;

                        // Create a new particle for the fly
                        int32 FlyParticleIndex = Particles.Emplace(FWebParticle(FlyLocation, false));
                        StuckFlies.Add(Fly, FlyParticleIndex);

                        // Attach the fly particle to the web
                        float Dist1 = FVector::Dist(FlyLocation, P1);
                        float Dist2 = FVector::Dist(FlyLocation, P2);
                        Constraints.Emplace(FWebConstraint(FlyParticleIndex, Constraint.Particle1Index, Dist1, 2.0f, false));
                        Constraints.Emplace(FWebConstraint(FlyParticleIndex, Constraint.Particle2Index, Dist2, 2.0f, false));

                        break; // Fly is stuck, move to the next fly
                    }
                }
            }
        }
        else if (Fly && Fly->bIsStuck && StuckFlies.Contains(Fly))
        {
            // Apply the fly's struggle force to the particle
            int32 ParticleIndex = StuckFlies[Fly];
            if (Particles.IsValidIndex(ParticleIndex))
            {
                FVector StruggleForce = FMath::VRand() * 5000.f; // Fly struggles randomly
                Particles[ParticleIndex].ApplyForce(StruggleForce);
            }
        }
    }
}


TArray<FVector> UWebPhysicsComponent::GetAndClearVibrationEvents()
{
    TArray<FVector> Events = MoveTemp(VibrationEvents);
    VibrationEvents.Empty();
    return Events;
}

void UWebPhysicsComponent::SolveConstraints()
{
    // The solver loop corrects particle positions to satisfy constraints
    for (int32 i = 0; i < SolverIterations; ++i)
    {
        for (int32 ConstraintIndex = 0; ConstraintIndex < Constraints.Num(); ++ConstraintIndex)
        {
            auto& Constraint = Constraints[ConstraintIndex];

            if (Constraint.bIsBroken)
            {
                continue;
            }

            if (Constraint.Particle1Index >= Particles.Num() || Constraint.Particle2Index >= Particles.Num())
            {
                // Safety check
                continue;
            }

            FWebParticle& P1 = Particles[Constraint.Particle1Index];
            FWebParticle& P2 = Particles[Constraint.Particle2Index];

            FVector Delta = P2.Position - P1.Position;
            float CurrentDistance = Delta.Size();

            if (CurrentDistance > 0.001f)
            {
                const float OldStress = LastFrameStress.Contains(ConstraintIndex) ? LastFrameStress[ConstraintIndex] : 0.f;
                const float StretchRatio = CurrentDistance / Constraint.RestLength;
                Constraint.Stress = FMath::Abs(StretchRatio - 1.0f);

                // Check for vibration
                if (FMath::Abs(Constraint.Stress - OldStress) > VibrationThreshold)
                {
                    VibrationEvents.Add((P1.Position + P2.Position) * 0.5f);
                }
                LastFrameStress.FindOrAdd(ConstraintIndex) = Constraint.Stress;


                if (StretchRatio > BreakThreshold && BreakThreshold > 0)
                {
                    Constraint.bIsBroken = true;
                    continue;
                }

                const float Error = CurrentDistance - Constraint.RestLength;
                const FVector Correction = Delta.GetSafeNormal() * Error * Constraint.Stiffness;

                if (!P1.bIsPinned && !P2.bIsPinned)
                {
                    P1.Position += Correction * 0.5f;
                    P2.Position -= Correction * 0.5f;
                }
                else if (!P1.bIsPinned)
                {
                    P1.Position += Correction;
                }
                else if (!P2.bIsPinned)
                {
                    P2.Position -= Correction;
                }
            }
        }
    }
}
