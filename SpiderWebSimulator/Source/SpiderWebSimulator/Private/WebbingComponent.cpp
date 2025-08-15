#include "WebbingComponent.h"
#include "ProceduralMeshComponent.h"

UWebbingComponent::UWebbingComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // This component doesn't need to tick on its own

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));

    CurrentMeshSectionIndex = 0;
}

void UWebbingComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UWebbingComponent::AddWebThread(const FVector& Start, const FVector& End, float Thickness)
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FProcMeshTangent> Tangents;
    TArray<FLinearColor> VertexColors;

    // Calculate the direction of the thread
    FVector Direction = (End - Start).GetSafeNormal();

    // Find a perpendicular vector. Using world Up vector for simplicity.
    // A more robust solution would handle cases where the thread is vertical.
    FVector Up = FVector::UpVector;
    if (FMath::Abs(FVector::DotProduct(Direction, Up)) > 0.99f)
    {
        Up = FVector::ForwardVector;
    }
    FVector Perpendicular = FVector::CrossProduct(Direction, Up).GetSafeNormal();

    float HalfThickness = Thickness / 2.0f;

    // Create a quad for the thread
    Vertices.Add(Start - Perpendicular * HalfThickness);
    Vertices.Add(Start + Perpendicular * HalfThickness);
    Vertices.Add(End + Perpendicular * HalfThickness);
    Vertices.Add(End - Perpendicular * HalfThickness);

    // Add triangles
    Triangles.Add(0);
    Triangles.Add(1);
    Triangles.Add(2);
    Triangles.Add(0);
    Triangles.Add(2);
    Triangles.Add(3);

    // Add UVs and Normals
    for (int i = 0; i < 4; ++i)
    {
        Normals.Add(Up); // Simple normal calculation
        VertexColors.Add(FLinearColor::White);
    }
    UVs.Add(FVector2D(0, 0));
    UVs.Add(FVector2D(1, 0));
    UVs.Add(FVector2D(1, 1));
    UVs.Add(FVector2D(0, 1));

    ProceduralMesh->CreateMeshSection_LinearColor(CurrentMeshSectionIndex, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

    CurrentMeshSectionIndex++;
}
