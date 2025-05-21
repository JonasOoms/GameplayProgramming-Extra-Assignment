#include "stdafx.h"
#include "EAStar.h"

using namespace Elite;

AStar::AStar(Graph* const pGraph, Heuristic hFunction)
	: m_pGraph(pGraph)
	, m_HeuristicFunction(hFunction)
{
}

std::vector<GraphNode*>AStar::FindPath(GraphNode* const pStartNode, GraphNode* const pGoalNode)
{
	std::vector<GraphNode*> path{};
	
	std::vector<NodeRecord> openNodes{};
	std::vector<NodeRecord> closedNodes{};

	NodeRecord startRecord;
	startRecord.pNode = pStartNode;
	startRecord.pConnection = nullptr;
	startRecord.costSoFar = 0.f;
	startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);

	openNodes.emplace_back(startRecord);

	while (!openNodes.empty())
	{
		auto currentIt = std::min_element(openNodes.begin(), openNodes.end(),
			[](const NodeRecord& a, const NodeRecord& b)
			{
				return a.estimatedTotalCost < b.estimatedTotalCost;
			});
		NodeRecord currentNode = *currentIt;

		// if we reach the end, reconstruct path
		if (currentNode.pNode == pGoalNode)
		{
			while (currentNode.pConnection != nullptr)
			{
				path.push_back(currentNode.pNode);
				auto fromNode = m_pGraph->GetNode(currentNode.pConnection->GetFromNodeId());
				auto it = std::find_if(closedNodes.begin(), closedNodes.end(), [&](const NodeRecord& record) { return record.pNode == fromNode; });
				if (it != closedNodes.end())
				{
					currentNode = *it;
				}
				else
				{
					break;
				}
			}

			path.emplace_back(pStartNode);
			std::reverse(path.begin(), path.end());
			return path;
		}

		openNodes.erase(currentIt);
		closedNodes.emplace_back(currentNode);

		auto connections = m_pGraph->GetConnectionsFromNode(currentNode.pNode->GetId());
		for (GraphConnection* pConnection : connections)
		{
			GraphNode* pToNode = m_pGraph->GetNode(pConnection->GetToNodeId());

			float gCost = currentNode.costSoFar + pConnection->GetCost();

			// check if the node is already in the closed list
			auto closedIt = std::find_if(closedNodes.begin(), closedNodes.end(), [&](const NodeRecord& record) { return record.pNode == pToNode; });
			if (closedIt != closedNodes.end())
			{
				if (gCost >= closedIt->costSoFar)
					continue;
				else
					// remove this node because a better path is found
					closedNodes.erase(closedIt);
			}

			// check if the node is in the open list
			auto openIt = std::find_if(openNodes.begin(), openNodes.end(), [&](const NodeRecord& record) { return record.pNode == pToNode; });
			if (openIt != openNodes.end())
			{
				if (gCost >= openIt->costSoFar)
					continue;
				else
					// remove this node because a better path is found
					openNodes.erase(openIt);
			}

			// Update the record
			NodeRecord neighborRecord;
			neighborRecord.pNode = pToNode;
			neighborRecord.pConnection = pConnection;
			neighborRecord.costSoFar = gCost;
			neighborRecord.estimatedTotalCost = gCost + GetHeuristicCost(pToNode, pGoalNode);
			openNodes.emplace_back(neighborRecord);
		}

		

	}

	return path;
}

float AStar::GetHeuristicCost(GraphNode* const pStartNode, GraphNode* const pEndNode) const
{
	Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
	return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
}