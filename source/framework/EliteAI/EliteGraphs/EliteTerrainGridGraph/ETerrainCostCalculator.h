#pragma once
#include "../EliteGridGraph/EConnectionCostCalculator.h"
#include "ETerrainGraphNode.h"

namespace Elite
{
	class TerrainCostCalculator : public ConnectionCostCalculator
	{
	public:
		TerrainCostCalculator() = default;
		~TerrainCostCalculator() = default;

		float CalculateConnectionCost(const Graph* const pGraph, int fromNodeId, int toNodeId) const override
		{
			TerrainGraphNode* fromNode = static_cast<TerrainGraphNode*>(pGraph->GetNode(fromNodeId));
			TerrainGraphNode* toNode = static_cast<TerrainGraphNode*>(pGraph->GetNode(toNodeId));
			
			float maxCost = static_cast<float>(std::max((int)fromNode->GetTerrainType(), (int)toNode->GetTerrainType()));
			return maxCost;
		}
	};
}