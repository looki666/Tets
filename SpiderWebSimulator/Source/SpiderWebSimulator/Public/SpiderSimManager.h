#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpiderSimManager.generated.h"

class ASpiderAIAgent;

UCLASS()
class SPIDERWEBSIMULATOR_API ASpiderSimManager : public AActor
{
    GENERATED_BODY()

public:
    ASpiderSimManager();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int32 NumberOfSpiders;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    TSubclassOf<ASpiderAIAgent> SpiderAgentClass;

private:
    void SpawnSpiders();
};
