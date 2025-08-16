#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPAction_MoveToLocation.generated.h"

class ASpiderAIAgent;

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPAction_MoveToLocation : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPAction_MoveToLocation();

    virtual void OnActionCreated(ASpiderAIAgent* NewOwner) override;
    virtual bool IsActionPossible() override;
    virtual bool PerformAction() override;
    virtual bool IsActionDone() override;
    virtual void ResetAction() override;

    void SetTargetLocation(const FVector& InTarget);

private:
    FVector TargetLocation;
    TArray<int32> Path;
    int32 CurrentPathIndex;

    // Helper function to find the closest particle to a world location
    int32 FindClosestParticle(const FVector& Location) const;
};
