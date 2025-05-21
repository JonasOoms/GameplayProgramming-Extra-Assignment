#include "stdafx.h"
#include "ENavGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraphNodeFactory/EGraphNodeFactory.h"
#include <algorithm>
//#include "../EliteGraphAlgorithms/EAStar.h"

using namespace Elite;

NavGraph::NavGraph(const std::vector<Polygon>& colliderShapes, float widthWorld, float heightWorld, float playerRadius = 1.0f) :
	Graph(false, new GraphNodeFactoryTemplate<NavGraphNode>()),
	m_pNavMeshPolygon(nullptr)
{
	float const halfWidth = widthWorld / 2.0f;
	float const halfHeight = heightWorld / 2.0f;
	std::list<Vector2> baseBox
	{ { -halfWidth, halfHeight },{ -halfWidth, -halfHeight },{ halfWidth, -halfHeight },{ halfWidth, halfHeight } };

	m_pNavMeshPolygon = new Polygon(baseBox); // Create copy on heap

	//Store all children
	for (Elite::Polygon p : colliderShapes)
	{
		p.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(p);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

NavGraph::NavGraph(const NavGraph& other): Graph(other)
{
}

NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon;
	m_pNavMeshPolygon = nullptr;
}

std::shared_ptr<NavGraph> NavGraph::Clone()
{
	return std::shared_ptr<NavGraph>(new NavGraph(*this));
}

int NavGraph::GetNodeIdFromLineIndex(int lineIdx) const
{
	for (auto& pNode : m_pNodes)
	{
		if (reinterpret_cast<NavGraphNode*>(pNode)->GetLineIndex() == lineIdx)
		{
			return pNode->GetId();
		}
	}

	return invalid_node_id;
}

Elite::Polygon* NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create a node on the center of each edge

	auto lines = m_pNavMeshPolygon->GetLines();

	for (Elite::Line* line : lines)
	{
		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index).size() <= 1)
		{
			continue;
		}

		Elite::Vector2 center = (line->p2 + line->p1) / 2.0f;
		auto pNode = new NavGraphNode(line->index,center);
		AddNode(pNode);
	}

	//2  Now that every node is created, connect the nodes that share the same triangle (for each triangle, ... )
	auto triangles = m_pNavMeshPolygon->GetTriangles();

	for (Elite::Triangle* triangle : triangles)
	{
		TriangleMetaData metaData = triangle->metaData;
		
		std::vector<int> nodeIndicesInTriangle;
		nodeIndicesInTriangle.reserve(3);


		for (auto lineIndex : metaData.IndexLines)
		{
			int nodeId = GetNodeIdFromLineIndex(lineIndex);
			if (nodeId != invalid_node_id)
			{
				nodeIndicesInTriangle.emplace_back(nodeId);
			}
		}

		if (nodeIndicesInTriangle.size() == 3)
		{
			AddConnection(new GraphConnection(nodeIndicesInTriangle[0], nodeIndicesInTriangle[1], 0));
			AddConnection(new GraphConnection(nodeIndicesInTriangle[1], nodeIndicesInTriangle[2], 0));
			AddConnection(new GraphConnection(nodeIndicesInTriangle[2], nodeIndicesInTriangle[0], 0));
		} 
		if (nodeIndicesInTriangle.size() == 2)
		{
			AddConnection(new GraphConnection(nodeIndicesInTriangle[0], nodeIndicesInTriangle[1], 0));
		}

	}


	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
}
