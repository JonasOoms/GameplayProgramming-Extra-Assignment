#pragma once

namespace Elite
{
	class Graph;
	class GraphNode;

	class BFS
	{
	public:
		BFS(Graph* const pGraph);

		std::vector<GraphNode*> FindPath(GraphNode* const pStartNode, GraphNode* const pDestinationNode);

	private:
		Graph* m_pGraph;
	};

}
