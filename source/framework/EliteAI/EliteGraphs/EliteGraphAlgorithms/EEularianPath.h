#pragma once
#include <stack>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include <algorithm>
#include <execution>
#include <ranges>

namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<GraphNode*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
		
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail


		// Count nodes with odd degree 


		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian


		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian
		// An Euler trail can be made, but only starting and ending in these 2 nodes


		// A connected graph with no odd nodes is Eulerian

		if (!IsConnected())
		{
			return Eulerianity::notEulerian;
		}

		auto& nodes = m_pGraph->GetAllNodes();
		int oddcount = 0;
		for (const auto& node : nodes)
		{
			auto& connections = m_pGraph->GetConnectionsFromNode(node->GetId());
			if (connections.size() & 1)
			{
				++oddcount;
			}
		}

		if (oddcount > 2)
		{
			return Eulerianity::notEulerian;
		}

		else if (oddcount == 2 && nodes.size() != 2)
		{
			return Eulerianity::semiEulerian;
		}

		return Eulerianity::eulerian;

	}

	inline std::vector<GraphNode*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		auto path = std::vector<GraphNode*>();
		int nrOfNodes = graphCopy->GetAmountOfNodes();

		// Check if there can be an Euler path
		eulerianity = IsEulerian();

		int index{ -1 };
		// Find a valid starting id for the algorithm
		
		// If this graph is not eulerian, return the empty path
		switch (eulerianity)
		{
		case Eulerianity::notEulerian:
			return path;
			break;
		case Eulerianity::semiEulerian:
			index = 0;
			break;
		case Eulerianity::eulerian:
			while (index < 0)
			{
				int randomIdx = Elite::RandomInt(nrOfNodes);
				if (graphCopy->GetNode(randomIdx))
				{
					index = randomIdx;
					break;
				}
			}
			break;
		}
		
		// Start algorithm loop
		std::stack<int> nodeStack;
		
		while (!(nodeStack.empty() && (graphCopy->GetConnectionsFromNode(index).empty())))
		{
			if (!graphCopy->GetConnectionsFromNode(index).empty())
			{
				nodeStack.push(index);

				auto connections = graphCopy->GetConnectionsFromNode(index);
				int randomConnectionIndex = Elite::RandomInt(connections.size());
				int selectedNeighbor = connections[randomConnectionIndex]->GetToNodeId();
				graphCopy->RemoveConnection(connections[randomConnectionIndex]);
				index = selectedNeighbor;
			}
			else
			{
				path.emplace_back(m_pGraph->GetNode(index));
				index = nodeStack.top();
				nodeStack.pop();
			}

		}

		path.emplace_back(m_pGraph->GetNode(index));

		std::reverse(path.begin(), path.end()); // reverses order of the path
		return path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex ) const
	{
		// mark the visited node
		
		// Ask the graph for the connections from that node
		
		// recursively visit any connected nodes that were not visited before

		// Tip: use an index-based for-loop to find the correct index

		visited[startIndex] = true;

		auto connections = m_pGraph->GetConnectionsFromNode(pNodes[startIndex]);

		for (GraphConnection* connection : connections) // Range-based loop for readability
		{
			int neighborIndex = connection->GetToNodeId(); // Get the ID of the connected node

			// Ensure the neighborIndex is within valid bounds before accessing visited
			if (neighborIndex >= 0 && neighborIndex < visited.size() && !visited[neighborIndex])
			{
				VisitAllNodesDFS(pNodes, visited, neighborIndex);
			}
		}

	}

	inline bool EulerianPath::IsConnected() const
	{
		auto nodes = m_pGraph->GetAllNodes();
		if (nodes.size() == 0)
			return false;

		// pick a starting node index
		
		int index{ -1 };
		while (index < 0)
		{
			int randomIdx = Elite::RandomInt(nodes.size());
			if (m_pGraph->GetNode(randomIdx))
			{
				index = randomIdx;
				break;
			}
		}
		
		//std::vector<GraphNode*> nodes = m_pGraph->GetAllNodes();
		std::vector<bool> visited;
		visited.resize(nodes.size(), false);

		VisitAllNodesDFS(nodes, visited, index);

		if (std::any_of(visited.begin(), visited.end(), [](bool v) { return !v; })) {
			return false;
		}
		return true;
		// if a node was never visited, this graph is not connected

	}
}