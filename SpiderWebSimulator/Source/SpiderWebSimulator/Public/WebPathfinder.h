#pragma once

#include "CoreMinimal.h"
#include "SpiderSimTypes.h"

// A* Node representation
struct FPathNode
{
    int32 ParticleIndex;
    float GCost; // Cost from start to this node
    float HCost; // Heuristic cost from this node to end
    int32 ParentIndex;

    float GetFCost() const { return GCost + HCost; }

    // Overload the > operator for the priority queue
    bool operator>(const FPathNode& Other) const
    {
        return GetFCost() > Other.GetFCost();
    }
};

class SPIDERWEBSIMULATOR_API FWebPathfinder
{
public:
    FWebPathfinder(const TArray<FWebParticle>* InParticles, const TArray<FWebConstraint>* InConstraints);

    // Finds a path from a start particle to a target particle
    bool FindPath(int32 StartParticleIndex, int32 TargetParticleIndex, TArray<int32>& OutPath);

private:
    TArray<int32> ReconstructPath(const TMap<int32, int32>& CameFrom, int32 Current);
    TArray<int32> GetNeighbors(int32 ParticleIndex) const;

    const TArray<FWebParticle>* Particles;
    const TArray<FWebConstraint>* Constraints;

    // Adjacency list for the web graph
    TMap<int32, TArray<int32>> AdjacencyList;
};
