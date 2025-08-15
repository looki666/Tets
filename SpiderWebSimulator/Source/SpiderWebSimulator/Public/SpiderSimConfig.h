#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpiderSimTypes.h"
#include "SpiderSimConfig.generated.h"

UCLASS(BlueprintType)
class SPIDERWEBSIMULATOR_API USpiderSimConfig : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // --- Physics ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    FRandomizedNumberParameter Gravity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    FRandomizedNumberParameter WindStrength;

    // --- AI ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    FRandomizedNumberParameter SpiderSpeed;

    // --- Web ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Web")
    FRandomizedNumberParameter WebBreakThreshold;

    // --- GOAP ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    float GoalChangeFrequency;

    USpiderSimConfig();
};
