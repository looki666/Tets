#include "WebPhysicsComponent.h"
#include "SpiderSimTypes.h"

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

    Integrate(DeltaTime);
    SolveConstraints();
}

void UWebPhysicsComponent::Initialize(TArray<FWebParticle>&& InParticles, TArray<FWebConstraint>&& InConstraints)
{
    Particles = MoveTemp(InParticles);
    Constraints = MoveTemp(InConstraints);
    LastFrameStress.Empty();
    VibrationEvents.Empty();
}

void UWebPhysicsComponent::ClearWeb()
{
    Particles.Empty();
    Constraints.Empty();
    LastFrameStress.Empty();
    VibrationEvents.Empty();
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
