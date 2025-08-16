#pragma once

#include "CoreMinimal.h"
#include "SpiderSimTypes.generated.h"

USTRUCT(BlueprintType)
struct SPIDERWEBSIMULATOR_API FRandomizedNumberParameter
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Randomized Number")
    float Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Randomized Number")
    bool bIsRandomized;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Randomized Number", meta = (EditCondition = "bIsRandomized"))
    float RandomFrom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Randomized Number", meta = (EditCondition = "bIsRandomized"))
    float RandomTo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Randomized Number")
    float Multiplier;

    FRandomizedNumberParameter()
        : Value(0.f)
        , bIsRandomized(false)
        , RandomFrom(0.f)
        , RandomTo(0.f)
        , Multiplier(1.f)
    {}

    // Helper function to get the final value
    float GetValue() const
    {
        float BaseValue = bIsRandomized ? FMath::RandRange(RandomFrom, RandomTo) : Value;
        return BaseValue * Multiplier;
    }
};

USTRUCT(BlueprintType)
struct SPIDERWEBSIMULATOR_API FWebParticle
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Particle")
    FVector Position;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Particle")
    FVector OldPosition;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Particle")
    FVector Acceleration;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Particle")
    bool bIsPinned;

    FWebParticle()
        : Position(FVector::ZeroVector)
        , OldPosition(FVector::ZeroVector)
        , Acceleration(FVector::ZeroVector)
        , bIsPinned(false)
    {}

    FWebParticle(const FVector& InPosition, bool InIsPinned = false)
        : Position(InPosition)
        , OldPosition(InPosition)
        , Acceleration(FVector::ZeroVector)
        , bIsPinned(InIsPinned)
    {}

    void ApplyForce(const FVector& Force)
    {
        if (!bIsPinned)
        {
            Acceleration += Force;
        }
    }
};

USTRUCT(BlueprintType)
struct SPIDERWEBSIMULATOR_API FWebConstraint
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Constraint")
    int32 Particle1Index;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Constraint")
    int32 Particle2Index;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Constraint")
    float RestLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Constraint")
    float Stiffness;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Constraint")
    bool bIsBroken;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Web Constraint")
    float Stress;

    FWebConstraint()
        : Particle1Index(0)
        , Particle2Index(0)
        , RestLength(0.f)
        , Stiffness(1.0f)
        , bIsBroken(false)
        , Stress(0.f)
    {}

    FWebConstraint(int32 InP1, int32 InP2, float InRestLength, float InStiffness = 1.0f)
        : Particle1Index(InP1)
        , Particle2Index(InP2)
        , RestLength(InRestLength)
        , Stiffness(InStiffness)
        , bIsBroken(false)
        , Stress(0.f)
    {}
};
