#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WebbingComponent.generated.h"

class UProceduralMeshComponent;
class UWebPhysicsComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPIDERWEBSIMULATOR_API UWebbingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWebbingComponent();

    // The main component for rendering the web mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Webbing")
    TObjectPtr<UProceduralMeshComponent> ProceduralMesh;

    // The physics component that drives the web simulation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Webbing")
    TObjectPtr<UWebPhysicsComponent> PhysicsComponent;

    // --- Visual Properties ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Webbing|Visuals")
    float ThreadThickness = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Webbing|Visuals")
    FLinearColor ThreadColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Webbing|Visuals")
    bool bShowStressColors = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Webbing|Visuals", meta = (EditCondition = "bShowStressColors"))
    FLinearColor LowStressColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Webbing|Visuals", meta = (EditCondition = "bShowStressColors"))
    FLinearColor HighStressColor = FLinearColor::Red;

protected:
    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

private:
    void UpdateWebMesh();
};
