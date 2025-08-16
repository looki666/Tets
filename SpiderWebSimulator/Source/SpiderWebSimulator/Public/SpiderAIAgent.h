#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOAPCore.h"
#include "SpiderAIAgent.generated.h"

class UWebbingComponent;
class UWebPhysicsComponent;
class UWebGenerator;
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
    TObjectPtr<UWebbingComponent> WebbingComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWebPhysicsComponent> PhysicsComponent;

    // --- AI Properties ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "AI")
    TObjectPtr<UWebGenerator> WebGenerator;

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

    int32 FindClosestParticleToLocation(const FVector& Location) const;

    // --- Public Data ---
    // This is populated by the Tick function when vibrations are detected
    TArray<FVector> VibrationEvents;

    // --- AI State Properties ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
    float Hunger = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
    float HungerThreshold = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
    float HungerDepletionRate = 0.1f;

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
