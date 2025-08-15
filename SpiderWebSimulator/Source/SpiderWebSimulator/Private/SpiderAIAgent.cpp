#include "SpiderAIAgent.h"
#include "WebbingComponent.h"

ASpiderAIAgent::ASpiderAIAgent()
{
    PrimaryActorTick.bCanEverTick = true;

    WebbingComponent = CreateDefaultSubobject<UWebbingComponent>(TEXT("WebbingComponent"));
}

void ASpiderAIAgent::BeginPlay()
{
    Super::BeginPlay();
}

void ASpiderAIAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    BuildWeb();
}

void ASpiderAIAgent::BuildWeb()
{
    // Web building logic will go here
}
