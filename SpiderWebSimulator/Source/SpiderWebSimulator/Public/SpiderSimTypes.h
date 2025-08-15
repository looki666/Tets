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
