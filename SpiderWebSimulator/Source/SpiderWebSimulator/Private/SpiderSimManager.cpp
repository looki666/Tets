#include "SpiderSimManager.h"
#include "SpiderAIAgent.h"
#include "Engine/World.h"

ASpiderSimManager::ASpiderSimManager()
{
    PrimaryActorTick.bCanEverTick = false;
    NumberOfSpiders = 5;
}

void ASpiderSimManager::BeginPlay()
{
    Super::BeginPlay();
    SpawnSpiders();
}

void ASpiderSimManager::SpawnSpiders()
{
    if (SpiderAgentClass)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            for (int32 i = 0; i < NumberOfSpiders; ++i)
            {
                FVector SpawnLocation = GetActorLocation() + FMath::VRand() * 500.0f;
                FRotator SpawnRotation = FRotator::ZeroRotator;
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();

                ASpiderAIAgent* NewAgent = World->SpawnActor<ASpiderAIAgent>(SpiderAgentClass, SpawnLocation, SpawnRotation, SpawnParams);
                if (NewAgent)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Spawned spider agent: %s"), *NewAgent->GetName());
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpiderAgentClass is not set in the SpiderSimManager."));
    }
}
