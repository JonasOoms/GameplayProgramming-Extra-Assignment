#include "stdafx.h"
#include "EGraph.h"
#include "EGraphNode.h"
#include "EGraphConnection.h"

using namespace Elite;

Graph::Graph(bool isDirectional, GraphNodeFactory* const pNodeFactory)
	:m_isDirectional{ isDirectional }
	, m_nextNodeId{ 0 }
	, m_pNodes{}
	, m_pConnections{}
	,m_pNodeFactory{pNodeFactory}
{
}

// Copy constructor
Graph::Graph(const Graph& other)
	:m_pNodes{  }
	, m_pConnections{ }
	, m_amountConnections{ other.m_amountConnections }
	, m_amountNodes{ other.m_amountNodes }
	, m_nextNodeId{ other.m_nextNodeId }
	, m_isDirectional{ other.m_isDirectional }
	, m_pNodeFactory{ other.m_pNodeFactory }
{
	for (std::vector<Elite::GraphConnection*>& connectionList : m_pConnections)
	{
		for (Elite::GraphConnection* pConnection : connectionList)
			SAFE_DELETE(pConnection);
	}

	for (Elite::GraphNode* pNode : other.m_pNodes)
	{
		if (pNode == nullptr)
		{
			m_pNodes.push_back(nullptr);
		}
		else
		{
			m_pNodes.push_back(CloneNode(*pNode));
		}
	}

	for (const std::vector<Elite::GraphConnection*>& connectionList : other.m_pConnections)
	{
		std::vector<GraphConnection*> newList;
		for (const Elite::GraphConnection* pConnection : connectionList)
			newList.push_back(new GraphConnection(*pConnection));
		m_pConnections.push_back(newList);
	}

	UpdateNextNodeIndex();
	UpdateActiveNodes();
}


Graph::~Graph()
{
	Clear();
}

//== NODES == 
int Graph::AddNode(GraphNode* const pNode)
{
	pNode->SetId(m_nextNodeId);
	const int size = static_cast<int>(m_pNodes.size());
	if (size < m_nextNodeId + 1)
	{
		m_pNodes.resize(m_nextNodeId + 1);
		m_pConnections.resize(m_nextNodeId + 1);
	}
	m_pNodes[pNode->GetId()] = pNode;

	++m_amountNodes;

	UpdateNextNodeIndex();
	UpdateActiveNodes();

	return pNode->GetId();
}

void Graph::Clear()
{
	for (Elite::GraphNode* pNode : m_pNodes)
	{
		SAFE_DELETE(pNode);
	}

	for (std::vector<Elite::GraphConnection*>& connections : m_pConnections)
	{
		for (Elite::GraphConnection* pConnection : connections)
		{
			SAFE_DELETE(pConnection);
		}
		connections.clear();
	}
	m_pConnections.clear();
	m_pNodes.clear();
	m_nextNodeId = 0;
}

GraphNode* const Graph::GetNode(int index) const
{
	if (!IsNodeValid(index))
		return nullptr;

	return m_pNodes[index];
}

bool Graph::IsNodeValid(int index) const
{
	return (size_t)index < m_pNodes.size() && m_pNodes[index] != nullptr;
}

void Graph::RemoveNode(int index)
{
	if (!IsNodeValid(index))
		return;

	GraphNode* node = m_pNodes[index];
	node->SetId(invalid_node_id);
	SAFE_DELETE(node);
	m_pNodes[index] = nullptr;

	--m_amountNodes;

	bool hadConnections{ false };

	//if the graph is not directed remove all connections leading to this pNode and then
	//clear the connections leading from the pNode
	if (!m_isDirectional)
	{
		//visit each neighbour and erase any connections leading to this pNode
		for (auto currentConnection = m_pConnections[index].begin();
			currentConnection != m_pConnections[index].end();
			++currentConnection)
		{
			for (auto currentEdgeOnToNode = m_pConnections[(*currentConnection)->GetToNodeId()].begin();
				currentEdgeOnToNode != m_pConnections[(*currentConnection)->GetToNodeId()].end();
				++currentEdgeOnToNode)
			{
				if ((*currentEdgeOnToNode)->GetToNodeId() == index)
				{
					hadConnections = true;

					Elite::GraphConnection* conPtr = *currentEdgeOnToNode;
					currentEdgeOnToNode = m_pConnections[(*currentConnection)->GetToNodeId()].erase(currentEdgeOnToNode);
					--m_amountConnections;
					SAFE_DELETE(conPtr);

					break;
				}
			}
		}
	}

	//finally, clear this pNode's connections
	for (Elite::GraphConnection* pConnection : m_pConnections[index])
	{
		--m_amountConnections;
		hadConnections = true;
		SAFE_DELETE(pConnection);
	}
	m_pConnections[index].clear();

	UpdateNextNodeIndex();
	UpdateActiveNodes();
}

int Graph::GetNodeIdAtPosition(const Vector2& pos, float errorMargin) const
{
	GraphNode* const pGraphNode = GetNodeAtPosition(pos, errorMargin);
	if (pGraphNode == nullptr)
		return invalid_node_id;

	return pGraphNode->GetId();
}

GraphNode* const Graph::GetNodeAtPosition(const Vector2& position, float errorMargin) const
{
	const float nodeRadiusSq = DEFAULT_NODE_RADIUS * DEFAULT_NODE_RADIUS * errorMargin * errorMargin;
	auto foundIt = find_if(m_pActiveNodes.begin(), m_pActiveNodes.end(),
		[position, nodeRadiusSq, this](GraphNode* pNode)
		{
			return pNode->GetId() != invalid_node_id && (pNode->GetPosition() - position).MagnitudeSquared() < nodeRadiusSq;
		});

	if (foundIt != m_pActiveNodes.end())
		return (*foundIt);
	else
		return nullptr;
}

void Graph::AddNodeAtIndex(GraphNode* const pNode, int index)
{
	pNode->SetId(m_nextNodeId);
	if (m_pNodes.size() < (size_t)m_nextNodeId + 1)
	{
		m_pNodes.resize(m_nextNodeId + 1);
		m_pConnections.resize(m_nextNodeId + 1);
	}
	m_pNodes[pNode->GetId()] = pNode;

	++m_amountNodes;

	UpdateNextNodeIndex();
	UpdateActiveNodes();
}

const std::vector<GraphNode*>& Graph::GetAllNodes() const
{
	return m_pActiveNodes;
}

//== CONNECTIONS ==
void Graph::AddConnection(GraphConnection* const pConnection)
{
	assert(IsNodeValid(pConnection->GetFromNodeId()) && IsNodeValid(pConnection->GetToNodeId()) && "<Graph::AddConnection>: invalid node index");

	m_pConnections[pConnection->GetFromNodeId()].push_back(pConnection);
	++m_amountConnections;

	if (!m_isDirectional)
	{
		GraphConnection* oppositeConn = new GraphConnection();
		oppositeConn->SetColor(pConnection->GetColor());
		oppositeConn->SetCost(pConnection->GetCost());
		oppositeConn->SetFromNodeId(pConnection->GetToNodeId());
		oppositeConn->SetToNodeId(pConnection->GetFromNodeId());

		m_pConnections[pConnection->GetToNodeId()].push_back(oppositeConn);
		++m_amountConnections;
	}
}

GraphConnection* const Graph::GetConnection(int from, int to) const
{
	assert(IsNodeValid(from) && "<Graph::GetConnection>: invalid 'from' node index");
	assert(IsNodeValid(to) && "<Graph::GetConnection>: invalid 'to' node index");
	for (auto c : m_pConnections[from])
	{
		if (c && c->GetToNodeId() == to)
			return c;
	}

	return nullptr;
}

void Graph::RemoveConnection(int from, int to)
{
	assert(IsNodeValid(from) && IsNodeValid(to));

	Elite::GraphConnection* conFromTo = GetConnection(from, to);
	Elite::GraphConnection* conToFrom = GetConnection(to, from);

	if (!m_isDirectional)
	{
		for (auto curEdge = m_pConnections[to].begin();
			curEdge != m_pConnections[to].end();
			++curEdge)
		{
			if ((*curEdge)->GetToNodeId() == from)
			{
				curEdge = m_pConnections[to].erase(curEdge);
				--m_amountConnections;
				break;
			}
		}
	}

	for (auto curEdge = m_pConnections[from].begin();
		curEdge != m_pConnections[from].end();
		++curEdge)
	{
		if ((*curEdge)->GetToNodeId() == to)
		{
			curEdge = m_pConnections[from].erase(curEdge);
			--m_amountConnections;
			break;
		}
	}

	SAFE_DELETE(conFromTo);
	SAFE_DELETE(conToFrom);

	OnGraphModified(false, true);
}

void Graph::RemoveConnection(GraphConnection* const pConnection)
{
	RemoveConnection(pConnection->GetFromNodeId(), pConnection->GetToNodeId());
}

void Elite::Graph::RemoveAllConnectionsWithNode(int nodeId)
{
	for (auto c : m_pConnections[nodeId])
		delete c;
	m_pConnections[nodeId].clear();

	// remove and delete connections from other nodes to this pNode
	auto isConnectionToThisNode = [nodeId](GraphConnection* const pCon) { return pCon->GetToNodeId() == nodeId; };
	for (std::vector<Elite::GraphConnection*>& c : m_pConnections)
	{
		std::vector<GraphConnection*>::iterator foundIt{};
		while ((foundIt = std::find_if(c.begin(), c.end(), isConnectionToThisNode)) != c.end())
		{
			delete* foundIt;
			c.erase(foundIt);
		}
	}

	OnGraphModified(false, true);
}

const std::vector<GraphConnection*>& Graph::GetConnectionsFromNode(int nodeId) const
{
	assert(IsNodeValid(nodeId));
	return m_pConnections[nodeId];
}

GraphConnection* const Graph::GetConnectionAtPosition(const Vector2& position, float maxDist) const
{
	GraphConnection* result = nullptr;
	float maxDistSq = maxDist * maxDist;

	for (const std::vector<Elite::GraphConnection*>& connectionList : m_pConnections)
	{
		for (Elite::GraphConnection* const pConnection : connectionList)
		{
			const Elite::Vector2 segmentStart = GetNode(pConnection->GetToNodeId())->GetPosition();
			const Elite::Vector2 segmentEnd = GetNode(pConnection->GetFromNodeId())->GetPosition();

			const Elite::Vector2 projectedPoint = ProjectOnLineSegment(segmentStart, segmentEnd, position);
			const float currentDistSq = DistanceSquared(projectedPoint, position);
			if (currentDistSq < maxDist * maxDist)
			{
				result = pConnection;
				maxDistSq = currentDistSq;
			}
		}
	}

	return result;
}

void Graph::SetConnectionCostsToDistances()
{
	for (const std::vector<Elite::GraphConnection*>& connections : m_pConnections)
	{
		for (Elite::GraphConnection* const pConnection : connections)
		{
			Vector2 fromPos = GetNode(pConnection->GetFromNodeId())->GetPosition();
			Vector2 toPos = GetNode(pConnection->GetToNodeId())->GetPosition();
			pConnection->SetCost(abs(Distance(fromPos, toPos)));
		}
	}
}

void Graph::UpdateNextNodeIndex()
{
	int idx = 0;
	while (true)
	{
		if (!IsNodeValid(idx))
		{
			m_nextNodeId = idx;
			return;
		}
		++idx;
	}
}

void Graph::UpdateActiveNodes()
{
	m_pActiveNodes.clear();
	for (auto& n : m_pNodes)
	{
		if (n != nullptr)
		{
			m_pActiveNodes.push_back(n);
		}
	}
}

std::shared_ptr<Graph> Graph::Clone() const
{
	return std::shared_ptr<Graph>(new Graph(*this));
}
