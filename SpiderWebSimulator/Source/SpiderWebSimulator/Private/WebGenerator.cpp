#include "WebGenerator.h"
#include "SpiderSimTypes.h"

UWebGenerator::UWebGenerator()
{
}

void UWebGenerator::GenerateWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    OutParticles.Empty();
    OutConstraints.Empty();

    switch (WebPattern)
    {
    case EWebPattern::ClassicOrb:
        GenerateClassicOrb(OutParticles, OutConstraints);
        break;
    case EWebPattern::Funnel:
        GenerateFunnelWeb(OutParticles, OutConstraints);
        break;
    case EWebPattern::Sheet:
        GenerateSheetWeb(OutParticles, OutConstraints);
        break;
    case EWebPattern::Cobweb:
        GenerateCobweb(OutParticles, OutConstraints);
        break;
    case EWebPattern::Triangular:
        GenerateTriangularWeb(OutParticles, OutConstraints);
        break;
    case EWebPattern::SpiralGalaxy:
        GenerateSpiralGalaxy(OutParticles, OutConstraints);
        break;
    case EWebPattern::RadialBurst:
        GenerateRadialBurst(OutParticles, OutConstraints);
        break;
    default:
        GenerateClassicOrb(OutParticles, OutConstraints);
    }
}

void UWebGenerator::GenerateClassicOrb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    TArray<TArray<int32>> RadialParticleIndices;
    RadialParticleIndices.SetNum(SpiralRings + 1);

    for (int32 r = 0; r <= SpiralRings; ++r)
    {
        float Radius = CenterHole + (WebSize - CenterHole) * FMath::Pow(static_cast<float>(r) / SpiralRings, SpiralSpacing);
        for (int32 i = 0; i < RadialThreads; ++i)
        {
            float Angle = (static_cast<float>(i) / RadialThreads) * 2.f * PI;
            float IrregularOffset = (FMath::FRand() - 0.5f) * Irregularity * Radius;
            float X = FMath::Cos(Angle) * (Radius + IrregularOffset);
            float Y = FMath::Sin(Angle) * (Radius + IrregularOffset);
            float Z = (FMath::FRand() - 0.5f) * ZDepth + FMath::Sin(Angle * 3.f) * ZDepth * 0.3f;

            bool bIsPinned = (r == SpiralRings && i % FMath::Max(1, RadialThreads / 8) == 0) || (r == 0 && RadialThreads > 8 && i % FMath::Max(1, RadialThreads / 4) == 0);
            int32 ParticleIndex = OutParticles.Emplace(FWebParticle(FVector(X, Y, Z), bIsPinned));
            RadialParticleIndices[r].Add(ParticleIndex);
        }
    }

    // Radial constraints
    for (int32 i = 0; i < RadialThreads; ++i)
    {
        for (int32 r = 0; r < SpiralRings; ++r)
        {
            int32 P1Index = RadialParticleIndices[r][i];
            int32 P2Index = RadialParticleIndices[r + 1][i];
            float Dist = FVector::Dist(OutParticles[P1Index].Position, OutParticles[P2Index].Position);
            OutConstraints.Emplace(FWebConstraint(P1Index, P2Index, Dist, 0.95f));
        }
    }

    // Spiral constraints
    for (int32 r = 0; r <= SpiralRings; ++r)
    {
        for (int32 i = 0; i < RadialThreads; ++i)
        {
            int32 P1Index = RadialParticleIndices[r][i];
            int32 P2Index = RadialParticleIndices[r][(i + 1) % RadialThreads];
            float Dist = FVector::Dist(OutParticles[P1Index].Position, OutParticles[P2Index].Position);
            OutConstraints.Emplace(FWebConstraint(P1Index, P2Index, Dist, 0.85f));
        }
    }
}

void UWebGenerator::GenerateFunnelWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    for (int32 l = 0; l <= SpiralRings; ++l)
    {
        float Depth = static_cast<float>(l) / SpiralRings * WebSize * 0.8f;
        float Radius = WebSize * (1.f - static_cast<float>(l) / SpiralRings * 0.7f);

        for (int32 i = 0; i < RadialThreads; ++i)
        {
            float Angle = (static_cast<float>(i) / RadialThreads) * 2.f * PI;
            float X = FMath::Cos(Angle) * Radius;
            float Y = FMath::Sin(Angle) * Radius;
            float Z = -Depth;

            bool bIsPinned = l == 0 && i % FMath::Max(1, RadialThreads / 6) == 0;
            OutParticles.Emplace(FWebParticle(FVector(X, Y, Z), bIsPinned));
        }
    }

    for (int32 l = 0; l < SpiralRings; ++l)
    {
        int32 Offset1 = l * RadialThreads;
        int32 Offset2 = (l + 1) * RadialThreads;

        for (int32 i = 0; i < RadialThreads; ++i)
        {
            int32 P1_Radial = Offset1 + i;
            int32 P2_Radial = Offset2 + i;
            float DistRadial = FVector::Dist(OutParticles[P1_Radial].Position, OutParticles[P2_Radial].Position);
            OutConstraints.Emplace(FWebConstraint(P1_Radial, P2_Radial, DistRadial));

            int32 P1_Circular = Offset1 + i;
            int32 P2_Circular = Offset1 + ((i + 1) % RadialThreads);
            float DistCircular = FVector::Dist(OutParticles[P1_Circular].Position, OutParticles[P2_Circular].Position);
            OutConstraints.Emplace(FWebConstraint(P1_Circular, P2_Circular, DistCircular));
        }
    }
}

void UWebGenerator::GenerateSheetWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    int32 Rows = SpiralRings;
    int32 Cols = RadialThreads;
    float Spacing = WebSize / FMath::Max(Rows, Cols);

    for (int32 r = 0; r <= Rows; ++r)
    {
        for (int32 c = 0; c <= Cols; ++c)
        {
            float X = (c - Cols / 2.f) * Spacing;
            float Y = (r - Rows / 2.f) * Spacing;
            float Z = FMath::Sin(X * 0.003f) * FMath::Cos(Y * 0.003f) * ZDepth + (FMath::FRand() - 0.5f) * Irregularity * 100.f;

            bool bIsPinned = (r == 0 || r == Rows) && (c == 0 || c == Cols);
            OutParticles.Emplace(FWebParticle(FVector(X, Y, Z), bIsPinned));
        }
    }

    for (int32 r = 0; r <= Rows; ++r)
    {
        for (int32 c = 0; c <= Cols; ++c)
        {
            int32 Index = r * (Cols + 1) + c;
            if (c < Cols)
            {
                int32 P1 = Index;
                int32 P2 = Index + 1;
                float Dist = FVector::Dist(OutParticles[P1].Position, OutParticles[P2].Position);
                OutConstraints.Emplace(FWebConstraint(P1, P2, Dist));
            }
            if (r < Rows)
            {
                int32 P1 = Index;
                int32 P2 = Index + Cols + 1;
                float Dist = FVector::Dist(OutParticles[P1].Position, OutParticles[P2].Position);
                OutConstraints.Emplace(FWebConstraint(P1, P2, Dist));
            }
        }
    }

    // Connect the last row horizontally
    TArray<int32>& LastRow = ParticleIndices[Levels];
    for (int32 i = 0; i < LastRow.Num() - 1; ++i)
    {
        int32 P1 = LastRow[i];
        int32 P2 = LastRow[i + 1];
        OutConstraints.Emplace(FWebConstraint(P1, P2, FVector::Dist(OutParticles[P1].Position, OutParticles[P2].Position)));
    }
}

void UWebGenerator::GenerateCobweb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    int32 Points = RadialThreads * 2;
    for (int32 i = 0; i < Points; ++i)
    {
        float X = (FMath::FRand() - 0.5f) * WebSize * 1.5f;
        float Y = (FMath::FRand() - 0.5f) * WebSize * 1.5f;
        float Z = (FMath::FRand() - 0.5f) * ZDepth * 2.f;
        bool bIsPinned = FMath::FRand() < 0.1f;
        OutParticles.Emplace(FWebParticle(FVector(X, Y, Z), bIsPinned));
    }

    for (int32 i = 0; i < OutParticles.Num(); ++i)
    {
        TArray<TTuple<float, int32>> Distances;
        for (int32 j = 0; j < OutParticles.Num(); ++j)
        {
            if (i == j) continue;
            Distances.Add(TTuple<float, int32>(FVector::Dist(OutParticles[i].Position, OutParticles[j].Position), j));
        }

        Distances.Sort();

        int32 Connections = FMath::Min(3 + FMath::RandHelper(2), Distances.Num());
        for (int32 c = 0; c < Connections; ++c)
        {
            int32 j = Distances[c].Value;
            bool bExists = OutConstraints.ContainsByPredicate([&](const FWebConstraint& Con) {
                return (Con.Particle1Index == i && Con.Particle2Index == j) || (Con.Particle1Index == j && Con.Particle2Index == i);
            });

            if (!bExists && Distances[c].Key < WebSize * 0.5f)
            {
                OutConstraints.Emplace(FWebConstraint(i, j, Distances[c].Key * (1.f + FMath::FRand() * Irregularity)));
            }
        }
    }
}

void UWebGenerator::GenerateTriangularWeb(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    TArray<TArray<int32>> ParticleIndices;
    int32 Levels = SpiralRings;

    for (int32 l = 0; l <= Levels; ++l)
    {
        TArray<int32> Row;
        int32 PointsInLevel = l + 1;
        for (int32 p = 0; p < PointsInLevel; ++p)
        {
            float t = (PointsInLevel > 1) ? static_cast<float>(p) / (PointsInLevel - 1) : 0.5f;
            float X = (t - 0.5f) * WebSize * (static_cast<float>(l) / Levels) * 2.f;
            float Y = -WebSize + (static_cast<float>(l) / Levels) * WebSize * 2.f;
            float Z = (FMath::FRand() - 0.5f) * ZDepth;

            bool bIsPinned = l == 0 || (l == Levels && (p == 0 || p == PointsInLevel - 1));
            int32 Index = OutParticles.Emplace(FWebParticle(FVector(X, Y, Z), bIsPinned));
            Row.Add(Index);
        }
        ParticleIndices.Add(Row);
    }

    for (int32 l = 0; l < Levels; ++l)
    {
        for (int32 i = 0; i < ParticleIndices[l].Num(); ++i)
        {
            int32 P1 = ParticleIndices[l][i];
            int32 P2_1 = ParticleIndices[l + 1][i];
            int32 P2_2 = ParticleIndices[l + 1][i + 1];

            OutConstraints.Emplace(FWebConstraint(P1, P2_1, FVector::Dist(OutParticles[P1].Position, OutParticles[P2_1].Position)));
            OutConstraints.Emplace(FWebConstraint(P1, P2_2, FVector::Dist(OutParticles[P1].Position, OutParticles[P2_2].Position)));

            if (i < ParticleIndices[l].Num() - 1)
            {
                int32 P_Horizontal = ParticleIndices[l][i+1];
                OutConstraints.Emplace(FWebConstraint(P1, P_Horizontal, FVector::Dist(OutParticles[P1].Position, OutParticles[P_Horizontal].Position)));
            }
        }
    }
}

void UWebGenerator::GenerateSpiralGalaxy(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    // Implementation left as an exercise for the reader :)
    // For now, it will just generate a classic orb web.
    GenerateClassicOrb(OutParticles, OutConstraints);
}

void UWebGenerator::GenerateRadialBurst(TArray<FWebParticle>& OutParticles, TArray<FWebConstraint>& OutConstraints)
{
    // Implementation left as an exercise for the reader :)
    // For now, it will just generate a classic orb web.
    GenerateClassicOrb(OutParticles, OutConstraints);
}
