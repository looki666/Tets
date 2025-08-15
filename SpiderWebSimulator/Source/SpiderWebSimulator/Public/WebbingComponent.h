#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WebbingComponent.generated.h"

class UProceduralMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPIDERWEBSIMULATOR_API UWebbingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWebbingComponent();

    // The main component for rendering the web mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Webbing")
    UProceduralMeshComponent* ProceduralMesh;

    // Adds a single thread to the web geometry
    UFUNCTION(BlueprintCallable, Category = "Webbing")
    void AddWebThread(const FVector& Start, const FVector& End, float Thickness);

protected:
    virtual void BeginPlay() override;

private:
    // Keeps track of the current mesh section index
    int32 CurrentMeshSectionIndex;
};
