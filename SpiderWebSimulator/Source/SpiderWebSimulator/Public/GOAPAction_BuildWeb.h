#pragma once

#include "CoreMinimal.h"
#include "GOAPCore.h"
#include "GOAPAction_BuildWeb.generated.h"

UCLASS()
class SPIDERWEBSIMULATOR_API UGOAPAction_BuildWeb : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPAction_BuildWeb();

    virtual bool IsActionPossible() override;
    virtual bool PerformAction() override;
    virtual void ResetAction() override;

private:
    bool bWebBuilt;
};
