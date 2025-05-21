#include "stdafx.h"
#include "EBFS.h"

#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"

using namespace Elite;

BFS::BFS(Graph* const pGraph)
	: m_pGraph(pGraph)
{
}

//Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<GraphNode*> BFS::FindPath(GraphNode* const pStartNode, GraphNode* const pDestinationNode)
{
	std::queue<GraphNode*> openList = {};
	std::map<GraphNode*, GraphNode*> closedList = {};

	openList.push(pStartNode);

	while (!openList.empty())
	{
		GraphNode* const pCurrentNode = openList.front();
		openList.pop();

		if (pCurrentNode == pDestinationNode) break;

		for (GraphConnection* const pConnection : m_pGraph->GetConnectionsFromNode(pCurrentNode->GetId()))
		{
			GraphNode* const pNextNode = m_pGraph->GetNode(pConnection->GetToNodeId());

			if (closedList.find(pNextNode) == closedList.end())
			{
				openList.push(pNextNode);
				closedList[pNextNode] = pCurrentNode;
			}

		}

	} 

	std::vector<GraphNode*> path{};
	GraphNode* pCurrentNode = pDestinationNode;

	while (pCurrentNode != pStartNode)
	{
		path.push_back(pCurrentNode);
		pCurrentNode = closedList.at(pCurrentNode);

	}
	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());
	



	return path;
}