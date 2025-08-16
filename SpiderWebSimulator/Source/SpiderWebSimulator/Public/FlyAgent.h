#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlyAgent.generated.h"

class USphereComponent;

UCLASS()
class SPIDERWEBSIMULATOR_API AFlyAgent : public AActor
{
    GENERATED_BODY()

public:
    AFlyAgent();

    //~ Begin AActor Interface
    virtual void Tick(float DeltaTime) override;
    //~ End AActor Interface

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // --- Movement Properties ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementSpeed = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WanderStrength = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FBox MovementBounds;

    // --- State Properties ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsStuck = false;

private:
    FVector CurrentVelocity;
    void UpdateMovement(float DeltaTime);
};
