#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPAction_HuntPrey.generated.h"

class AFlyAgent;

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPAction_HuntPrey : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPAction_HuntPrey();

    virtual bool IsActionPossible() override;
    virtual bool PerformAction() override;
    virtual bool IsActionDone() override;
    virtual void ResetAction() override;

private:
    TArray<int32> Path;
    int32 CurrentPathIndex;
    bool bIsAtDestination;

    UPROPERTY()
    TObjectPtr<AFlyAgent> TargetPrey;
};
