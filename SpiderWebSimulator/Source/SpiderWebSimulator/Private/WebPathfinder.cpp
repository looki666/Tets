#include "WebPathfinder.h"
#include "SpiderSimTypes.h"

FWebPathfinder::FWebPathfinder(const TArray<FWebParticle>* InParticles, const TArray<FWebConstraint>* InConstraints)
    : Particles(InParticles)
    , Constraints(InConstraints)
{
    // Pre-build the adjacency list for the graph
    if (Particles && Constraints)
    {
        for (const auto& Constraint : *Constraints)
        {
            if (!Constraint.bIsBroken)
            {
                AdjacencyList.FindOrAdd(Constraint.Particle1Index).Add(Constraint.Particle2Index);
                AdjacencyList.FindOrAdd(Constraint.Particle2Index).Add(Constraint.Particle1Index);
            }
        }
    }
}

bool FWebPathfinder::FindPath(int32 StartParticleIndex, int32 TargetParticleIndex, TArray<int32>& OutPath)
{
    if (!Particles || !Particles->IsValidIndex(StartParticleIndex) || !Particles->IsValidIndex(TargetParticleIndex))
    {
        return false;
    }

    TArray<FPathNode> OpenSet; // Priority Queue
    TMap<int32, float> GScore; // Cost from start along best known path
    TMap<int32, int32> CameFrom;

    GScore.FindOrAdd(StartParticleIndex) = 0;

    FPathNode StartNode;
    StartNode.ParticleIndex = StartParticleIndex;
    StartNode.GCost = 0;
    StartNode.HCost = FVector::Dist((*Particles)[StartParticleIndex].Position, (*Particles)[TargetParticleIndex].Position);
    StartNode.ParentIndex = -1;
    OpenSet.HeapPush(StartNode);

    while (OpenSet.Num() > 0)
    {
        FPathNode CurrentNode;
        OpenSet.HeapPop(CurrentNode);

        if (CurrentNode.ParticleIndex == TargetParticleIndex)
        {
            OutPath = ReconstructPath(CameFrom, CurrentNode.ParticleIndex);
            return true;
        }

        for (int32 NeighborIndex : GetNeighbors(CurrentNode.ParticleIndex))
        {
            float TentativeGScore = GScore[CurrentNode.ParticleIndex] + FVector::Dist((*Particles)[CurrentNode.ParticleIndex].Position, (*Particles)[NeighborIndex].Position);

            if (!GScore.Contains(NeighborIndex) || TentativeGScore < GScore[NeighborIndex])
            {
                CameFrom.FindOrAdd(NeighborIndex) = CurrentNode.ParticleIndex;
                GScore.FindOrAdd(NeighborIndex) = TentativeGScore;

                FPathNode NeighborNode;
                NeighborNode.ParticleIndex = NeighborIndex;
                NeighborNode.GCost = TentativeGScore;
                NeighborNode.HCost = FVector::Dist((*Particles)[NeighborIndex].Position, (*Particles)[TargetParticleIndex].Position);
                NeighborNode.ParentIndex = CurrentNode.ParticleIndex;

                OpenSet.HeapPush(NeighborNode);
            }
        }
    }

    return false; // No path found
}

TArray<int32> FWebPathfinder::ReconstructPath(const TMap<int32, int32>& CameFrom, int32 Current)
{
    TArray<int32> TotalPath;
    TotalPath.Add(Current);
    while (CameFrom.Contains(Current))
    {
        Current = CameFrom[Current];
        TotalPath.Insert(Current, 0);
    }
    return TotalPath;
}

TArray<int32> FWebPathfinder::GetNeighbors(int32 ParticleIndex) const
{
    if (AdjacencyList.Contains(ParticleIndex))
    {
        return AdjacencyList[ParticleIndex];
    }
    return TArray<int32>();
}
