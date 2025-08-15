#include "BuildTestThreadAction.h"
#include "SpiderAIAgent.h"
#include "WebbingComponent.h"

UBuildTestThreadAction::UBuildTestThreadAction()
{
    Preconditions.States.Add(FName("HasSilk"), true);
    Effects.States.Add(FName("HasBuiltTestThread"), true);
    Cost = 1.0f;
    bPerformed = false;
}

bool UBuildTestThreadAction::IsActionDone_Implementation()
{
    return bPerformed;
}

bool UBuildTestThreadAction::PerformAction_Implementation()
{
    if (Agent && Agent->WebbingComponent)
    {
        FVector Start = Agent->GetActorLocation();
        FVector End = Start + FVector(100, 100, 50); // Hardcoded for testing
        Agent->WebbingComponent->AddWebThread(Start, End, 1.0f);

        bPerformed = true;
        return true;
    }
    return false;
}

void UBuildTestThreadAction::ResetAction_Implementation()
{
    bPerformed = false;
}
