#include "stdafx.h"

#include "ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"

using namespace Elite;

std::vector<Vector2> NavMeshPathfinding::FindPath(const Vector2& startPos, const Vector2& endPos, 
	NavGraph* const pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
{
	//Create the path to return
	std::vector<Vector2> finalPath{};

	//Get the startTriangle and endTriangle
	const Triangle* startTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos);
	const Triangle* endTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos);
\
	//If we don't have a valid startTriangle or endTriangle -> return empty path
	if (startTriangle == nullptr || endTriangle == nullptr)
	{
		return finalPath;
	}
	//If the startTriangle and endTriangle are the same -> return straight line path
	if (startTriangle == endTriangle)
	{
	finalPath.emplace_back(startPos);
	finalPath.emplace_back(endPos);
	return finalPath;
	}


	//=> Start looking for a path
	//Clone the graph (returns shared_ptr!)
	std::shared_ptr<Elite::NavGraph> pNavGraphClone = pNavGraph->Clone();

	//Create extra node for the Start Node (Agent's position) and add it to the graph. 
	
	int startPositionNodeId = pNavGraphClone->AddNode(new NavGraphNode(-1,startPos));

	//Make connections between the Start Node and the startTriangle nodes.
	for (int lineId : startTriangle->metaData.IndexLines)
	{
		int nodeId = pNavGraphClone->GetNodeIdFromLineIndex(lineId);

		if (nodeId != invalid_node_id)
		{
			pNavGraphClone->AddConnection(new GraphConnection(startPositionNodeId, nodeId));
		}
		
	}

	//Create extra node for the End Node (endpos) and add it to the graph. 
	int endPositionNodeId = pNavGraphClone->AddNode(new NavGraphNode(-1,endPos));
	//Make connections between the End Node and the endTriangle nodes.

	for (int lineId : endTriangle->metaData.IndexLines)
	{
		int nodeId = pNavGraphClone->GetNodeIdFromLineIndex(lineId);

		if (nodeId != invalid_node_id)
		{
			pNavGraphClone->AddConnection(new GraphConnection(endPositionNodeId, nodeId));
		}

	}
	pNavGraphClone->SetConnectionCostsToDistances();


	//Run AStar on the new graph
	Elite::AStar aStar{ pNavGraphClone.get(), Elite::HeuristicFunctions::Chebyshev};

	std::vector<GraphNode*> calculatedPath = aStar.FindPath(pNavGraphClone->GetNode(startPositionNodeId), pNavGraphClone->GetNode(endPositionNodeId));
	
	for (auto node : calculatedPath)
	{
		finalPath.emplace_back(node->GetPosition());
	}
	debugNodePositions = finalPath;
	//Run optimiser on new graph, MAKE SURE the AStar path is working properly before starting the following section:
	debugPortals = SSFA::FindPortals(calculatedPath, pNavGraph->GetNavMeshPolygon());
	finalPath = SSFA::OptimizePortals(debugPortals);

	return finalPath;
}

std::vector<Vector2> Elite::NavMeshPathfinding::FindPath(const Vector2& startPos, const Vector2& endPos, NavGraph* const pNavGraph)
{
	std::vector<Vector2> debugNodePositions{};
	std::vector<Portal> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}