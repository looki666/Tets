#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpiderSimTypes.h"
#include "WebGenerator.generated.h"

UENUM(BlueprintType)
enum class EWebPattern : uint8
{
    ClassicOrb    UMETA(DisplayName = "Classic Orb"),
    Funnel        UMETA(DisplayName = "Funnel Web"),
    Sheet         UMETA(DisplayName = "Sheet Web"),
    Cobweb        UMETA(DisplayName = "Cobweb"),
    Triangular    UMETA(DisplayName = "Triangular"),
    SpiralGalaxy  UMETA(DisplayName = "Spiral Galaxy"),
    RadialBurst   UMETA(DisplayName = "Radial Burst")
};

UCLASS(Blueprintable, BlueprintType)
class SPIDERWEBSIMULATOR_API UWebGenerator : public UObject
{
    GENERATED_BODY()

public:
    UWebGenerator();

    // --- Generation Parameters ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation")
    EWebPattern WebPattern = EWebPattern::ClassicOrb;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "4", ClampMax = "64"))
    int32 RadialThreads = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "3", ClampMax = "50"))
    int32 SpiralRings = 15;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "1.0", ClampMax = "50.0"))
    float WebSize = 1200.f; // Increased scale for Unreal units

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float CenterHole = 100.f; // Increased scale

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Irregularity = 0.08f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "0.5", ClampMax = "3.0"))
    float SpiralSpacing = 1.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Web Generation", meta = (ClampMin = "0.0", ClampMax = "500.0"))
    float ZDepth = 30.f; // Increased scale

    // --- Public Functions ---
    UFUNCTION(BlueprintCallable, Category = "Web Generation")
    void GenerateWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);

private:
    // --- Private Generation Logic ---
    void GenerateClassicOrb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
    void GenerateFunnelWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
    void GenerateSheetWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
    void GenerateCobweb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
    void GenerateTriangularWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
    void GenerateSpiralGalaxy(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
    void GenerateRadialBurst(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints);
};
