#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WebbingComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPIDERWEBSIMULATOR_API UWebbingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWebbingComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void GenerateWeb();
};
