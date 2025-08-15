#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpiderAIAgent.generated.h"

class UWebbingComponent;

UCLASS()
class SPIDERWEBSIMULATOR_API ASpiderAIAgent : public AActor
{
    GENERATED_BODY()

public:
    ASpiderAIAgent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWebbingComponent* WebbingComponent;

private:
    void BuildWeb();
};
