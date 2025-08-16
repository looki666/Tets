#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpiderSimTypes.h"
#include "WebPhysicsComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPIDERWEBSIMULATOR_API UWebPhysicsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWebPhysicsComponent();

    //~ Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

    // --- Physics Properties ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Physics")
    int32 SolverIterations = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Physics")
    float Damping = 0.98f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Physics")
    FVector Gravity = FVector(0.f, 0.f, -980.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Physics")
    float BreakThreshold = 8.0f;

    // --- Public Data Access ---
    TArray<FWebParticle> Particles;
    TArray<FWebConstraint> Constraints;

    // --- Public Functions ---
    void Initialize(TArray<FWebParticle>&& InParticles, TArray<FWebConstraint>&& InConstraints);
    void ClearWeb();

public:
    // --- Public Event Data ---
    TArray<FVector> GetAndClearVibrationEvents();

private:
    // --- Private Simulation Logic ---
    void Integrate(float DeltaTime);
    void SolveConstraints();

    // --- Private Event Data ---
    TArray<FVector> VibrationEvents;
    TMap<int32, float> LastFrameStress;

    UPROPERTY(EditAnywhere, Category = "Web Physics|Events")
    float VibrationThreshold = 0.5f;
};
