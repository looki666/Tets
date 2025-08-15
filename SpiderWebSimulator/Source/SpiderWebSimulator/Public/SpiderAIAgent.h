#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOAPCore.h"
#include "SpiderAIAgent.generated.h"

class UWebbingComponent;
class UGOAPPlanner;
class UGOAPAction;
class UGOAPGoal;
class USpiderSimConfig;

UCLASS()
class SPIDERWEBSIMULATOR_API ASpiderAIAgent : public AActor
{
    GENERATED_BODY()

public:
    ASpiderAIAgent();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWebbingComponent* WebbingComponent;

    // --- GOAP Properties ---

    // The agent's current knowledge of the world state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GOAP")
    FWorldState CurrentWorldState;

    // The set of actions this agent can perform. Can be configured in Blueprints.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "GOAP")
    TSet<UGOAPAction*> AvailableActions;

    // The set of goals this agent can pursue. Can be configured in Blueprints.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "GOAP")
    TSet<UGOAPGoal*> AvailableGoals;

    // The current active plan
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GOAP")
    TArray<UGOAPAction*> CurrentPlan;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --- Public Functions ---
    void SetConfig(USpiderSimConfig* InConfig);

    UFUNCTION(BlueprintCallable, Category = "GOAP")
    void UpdateWorldState(FName Key, bool Value);

private:
    // --- Private GOAP Logic ---
    void UpdateGOAP();
    void FindNewPlan();
    void ExecuteCurrentAction();

    UPROPERTY()
    UGOAPPlanner* Planner;

    UPROPERTY()
    UGOAPGoal* CurrentGoal;

    UPROPERTY()
    USpiderSimConfig* Config;

    float TimeSinceLastPlan;
};
