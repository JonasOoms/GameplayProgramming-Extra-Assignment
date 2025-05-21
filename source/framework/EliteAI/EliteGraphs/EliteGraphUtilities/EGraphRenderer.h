#pragma once

#include  <type_traits>
#include "EGraphVisuals.h"

namespace Elite
{
	class GraphNode;
	class GraphConnection;
	class Graph;
	class GridGraph;
	class TerrainGraphNode;

	struct GraphRenderingOptions final
	{
		GraphRenderingOptions() = default;
		GraphRenderingOptions(bool renderNodes = true, bool renderNodesTxt = false, bool renderConnections = true, bool renderConnectionsTxt = false):
			renderNodes{ renderNodes },
			renderConnections{ renderConnections },
			renderNodesTxt{renderNodesTxt},
			renderConnectionsTxt{ renderConnectionsTxt }
		{
		}
		bool renderNodes;
		bool renderNodesTxt;
		bool renderConnections;
		bool renderConnectionsTxt;
	};

	class GraphRenderer final
	{
	public:
		GraphRenderer() = default;
		~GraphRenderer() = default;

		void RenderGraph(Graph* const pGraph, const GraphRenderingOptions& renderOptions) const;
		void SetNumberPrintPrecision(int precision) { m_FloatPrintPrecision = precision; }
		void RenderGraph(GridGraph* const pGraph, bool renderNodes, bool renderNodeTxt, bool renderConnections, bool renderConnectionsCosts) const;
		void HighlightNodes(GridGraph* const pGraph, const std::vector<GraphNode*>& path, Color col = HIGHLIGHTED_NODE_COLOR) const;

	private:
		void RenderCircleNode(const Vector2& pos, const std::string& text = "", float radius = DEFAULT_NODE_RADIUS, const Elite::Color& col = DEFAULT_NODE_COLOR, float depth = 0.0f) const;
		void RenderRectNode(const Vector2& pos, const std::string& text = "", float width = DEFAULT_NODE_RADIUS, const Elite::Color& col = DEFAULT_NODE_COLOR, float depth = 0.0f) const;
		void RenderConnection(GraphConnection* con, const Vector2& toPos, const Elite::Vector2& fromPos, const std::string& text, const Elite::Color& col = DEFAULT_CONNECTION_COLOR, float depth = 0.0f) const;

		// Get correct color/text depending on the pNode/pConnection type
		//template<class T_NodeType, typename = typename std::enable_if<! std::is_base_of<GraphNode2D, T_NodeType>::value>::type>
		Elite::Color GetNodeColor(GraphNode* const pNode) const;
		Elite::Color GetConnectionColor(GraphConnection* const pConnection) const;
		std::string GetNodeText(GraphNode* const pNode) const;
		std::string GetConnectionText(GraphConnection* const pConnection) const;

		//C++ make the class non-copyable
		GraphRenderer(const GraphRenderer&) = delete;
		GraphRenderer& operator=(const GraphRenderer&) = delete;

		// variables
		int m_FloatPrintPrecision = 1;
	};
}
