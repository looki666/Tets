#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPCore.generated.h"

// Represents a key-value pair state of the world
USTRUCT(BlueprintType)
struct SPIDERWEBSIMULATOR_API FWorldState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    TMap<FName, bool> States;

    bool operator==(const FWorldState& Other) const
    {
        return States.OrderIndependentCompareEqual(Other.States);
    }
};

/**
 * Base class for all GOAP Actions.
 * Actions are the building blocks of plans.
 */
UCLASS(Blueprintable, Abstract)
class SPIDERWEBSIMULATOR_API UGOAPAction : public UObject
{
    GENERATED_BODY()

public:
    UGOAPAction();

    // The state of the world required for this action to run
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    FWorldState Preconditions;

    // The resulting state of the world after this action runs
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    FWorldState Effects;

    // The cost of performing this action
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    float Cost;

    // The agent performing the action
    UPROPERTY()
    class ASpiderAIAgent* Agent;

    // --- Virtual Functions for Action Execution ---

    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    void OnActionCreated(class ASpiderAIAgent* ActionAgent);

    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    bool IsActionDone();

    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    bool PerformAction();

    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    void ResetAction();
};

/**
 * Base class for all GOAP Goals.
 * Goals represent the desired state of the world for an agent.
 */
UCLASS(Blueprintable, Abstract)
class SPIDERWEBSIMULATOR_API UGOAPGoal : public UObject
{
    GENERATED_BODY()

public:
    // The desired state of the world
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    FWorldState DesiredState;

    // The priority of this goal. Higher values are more important.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    float Priority;

    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    float GetPriority();
};
