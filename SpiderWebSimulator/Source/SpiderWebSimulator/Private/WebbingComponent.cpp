#include "WebbingComponent.h"
#include "WebPhysicsComponent.h"
#include "ProceduralMeshComponent.h"
#include "SpiderSimTypes.h"

UWebbingComponent::UWebbingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
}

void UWebbingComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!PhysicsComponent)
    {
        PhysicsComponent = GetOwner()->FindComponentByClass<UWebPhysicsComponent>();
    }
}

void UWebbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateWebMesh();
}

void UWebbingComponent::UpdateWebMesh()
{
    if (!PhysicsComponent || !ProceduralMesh || PhysicsComponent->Particles.Num() == 0)
    {
        ProceduralMesh->ClearAllMeshSections();
        return;
    }

    TArray<FVector> AllVertices;
    TArray<int32> AllTriangles;
    TArray<FVector> AllNormals;
    TArray<FVector2D> AllUVs;
    TArray<FLinearColor> AllVertexColors;
    TArray<FProcMeshTangent> AllTangents; // This can be left empty if not needed

    int32 VertexIndex = 0;
    for (const auto& Constraint : PhysicsComponent->Constraints)
    {
        if (Constraint.bIsBroken)
        {
            continue;
        }

        const FWebParticle& P1 = PhysicsComponent->Particles[Constraint.Particle1Index];
        const FWebParticle& P2 = PhysicsComponent->Particles[Constraint.Particle2Index];

        FVector Direction = (P2.Position - P1.Position).GetSafeNormal();
        FVector Up = FVector::UpVector;
        if (FMath::Abs(FVector::DotProduct(Direction, Up)) > 0.99f)
        {
            Up = FVector::ForwardVector;
        }
        FVector Perpendicular = FVector::CrossProduct(Direction, Up).GetSafeNormal();

        float HalfThickness = ThreadThickness / 2.0f;

        // Add the 4 vertices for this thread quad
        AllVertices.Add(P1.Position - Perpendicular * HalfThickness);
        AllVertices.Add(P1.Position + Perpendicular * HalfThickness);
        AllVertices.Add(P2.Position + Perpendicular * HalfThickness);
        AllVertices.Add(P2.Position - Perpendicular * HalfThickness);

        // Add the 6 triangle indices for this quad
        AllTriangles.Add(VertexIndex + 0);
        AllTriangles.Add(VertexIndex + 1);
        AllTriangles.Add(VertexIndex + 2);
        AllTriangles.Add(VertexIndex + 0);
        AllTriangles.Add(VertexIndex + 2);
        AllTriangles.Add(VertexIndex + 3);

        VertexIndex += 4; // Increment the base vertex index for the next quad

        FLinearColor Color = ThreadColor;
        if (bShowStressColors)
        {
            Color = FLinearColor::LerpUsingHSV(LowStressColor, HighStressColor, FMath::Clamp(Constraint.Stress * 5.f, 0.f, 1.f));
        }

        for (int i = 0; i < 4; ++i)
        {
            AllNormals.Add(Up);
            AllVertexColors.Add(Color);
        }
        AllUVs.Add(FVector2D(0, 0));
        AllUVs.Add(FVector2D(1, 0));
        AllUVs.Add(FVector2D(1, 1));
        AllUVs.Add(FVector2D(0, 1));
    }

    // Now create or update a single mesh section with all the geometry
    if (AllVertices.Num() > 0)
    {
        ProceduralMesh->CreateMeshSection_LinearColor(0, AllVertices, AllTriangles, AllNormals, AllUVs, AllVertexColors, AllTangents, true);
    }
    else
    {
        ProceduralMesh->ClearAllMeshSections();
    }
}
