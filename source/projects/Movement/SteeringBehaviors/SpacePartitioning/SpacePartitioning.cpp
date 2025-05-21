#include "stdafx.h"
#include "SpacePartitioning.h"
#include <algorithm>
#include <execution>
#include "../Flocking/Flock.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	//Calculate bounds of a cell
	m_CellWidth = width / cols;
	m_CellHeight = height / rows;

	for (int rowIdx{}; rowIdx < rows; ++rowIdx)
	{
		for (int columnIdx{}; columnIdx < cols; ++columnIdx)
		{
			m_Cells.emplace_back(columnIdx * m_CellWidth, rowIdx * m_CellHeight, m_CellWidth, m_CellHeight);
		}
	}
}

void CellSpace::AddAgent(SteeringAgent* const agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.emplace_back(agent);
}

void CellSpace::AgentPositionChanged(SteeringAgent* const agent, const Elite::Vector2& oldPos)
{
	int newIndex = PositionToIndex(agent->GetPosition());
	int oldIndex = PositionToIndex(oldPos);

	if (newIndex == oldIndex) return;

	m_Cells[oldIndex].agents.remove(agent);
	m_Cells[newIndex].agents.emplace_back(agent);
}

void CellSpace::RegisterNeighbors(SteeringAgent* const pAgent, float neighborhoodRadius)
{
	Elite::Vector2 agentPos = pAgent->GetPosition();
	m_NrOfNeighbors = 0;
	float neighborhoodRadiusSq = neighborhoodRadius * neighborhoodRadius;
	m_NeighborhoodRadius = neighborhoodRadius;

	int minCol = std::max(0, int((agentPos.x - neighborhoodRadius) / m_CellWidth));
	int maxCol = std::min(m_NrOfCols-1,int((agentPos.x + neighborhoodRadius) / m_CellWidth));
	int minRow = std::max(0,int((agentPos.y - neighborhoodRadius) / m_CellHeight));
	int maxRow = std::min(m_NrOfRows-1,int((agentPos.y + neighborhoodRadius) / m_CellHeight));

	for (int row{ minRow }; row <= maxRow; ++row)
	{
		for (int col{ minCol }; col <= maxCol; ++col)
		{
			int cellIdx = row * m_NrOfCols + col;
			Cell& cell = m_Cells[cellIdx];

			// redundant corner check (if cell is not in the circle)
			float cellCenterX = (col + 0.5f) * m_CellWidth;
			float cellCenterY = (row + 0.5f) * m_CellHeight;
			Elite::Vector2 cellCenter{ cellCenterX, cellCenterY };
			float distSqToCellCenter = Elite::DistanceSquared(agentPos, cellCenter);
			float cellHalfDiag = 0.5f * std::sqrt(m_CellWidth * m_CellWidth + m_CellHeight * m_CellHeight);
			float maxCellDist = neighborhoodRadius + cellHalfDiag;
			if (distSqToCellCenter > maxCellDist * maxCellDist)
			{
				continue;
			}

			for (const auto& agent : cell.agents)
			{
				float distanceSq = Elite::DistanceSquared(agent->GetPosition(), pAgent->GetPosition());
				if (distanceSq < neighborhoodRadiusSq)
				{
					m_Neighbors[m_NrOfNeighbors++] = agent;
				}
			}
		}
	}
}

void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.agents.clear();
}

void CellSpace::RenderCells(bool doDrawNeighborCells, SteeringAgent* pAgent, Flock* flock) const
{

	
	if (doDrawNeighborCells)
	{
		Elite::Rect neighborBox{ Elite::Vector2(pAgent->GetPosition().x - m_NeighborhoodRadius, pAgent->GetPosition().y - m_NeighborhoodRadius), (float) 2 * m_NeighborhoodRadius, (float) 2 * m_NeighborhoodRadius };

		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_NeighborhoodRadius, Elite::Color(0, 1, 0), 0.7);
		DEBUGRENDERER2D->DrawPolygon(std::vector<Elite::Vector2>{ neighborBox.bottomLeft,
									neighborBox.bottomLeft + Elite::Vector2(0, neighborBox.height),
									neighborBox.bottomLeft + Elite::Vector2(neighborBox.width, neighborBox.height),
									neighborBox.bottomLeft + Elite::Vector2(neighborBox.width, 0) }.data(), 4, Elite::Color(0, 0, 1), 0.7f);

		for (int cellIndex{}; cellIndex < m_NrOfCols * m_NrOfRows; ++cellIndex)
		{
			Elite::Color drawColor{ 1,0,0 };
			float priority{ 0.1f };
			if (Elite::IsOverlapping(m_Cells[cellIndex].boundingBox, neighborBox))
			{
				drawColor = Elite::Color{ 1,1,0 };
				priority = 0.01f;
			}
			DEBUGRENDERER2D->DrawPolygon(m_Cells[cellIndex].GetRectPoints().data(), m_Cells[cellIndex].GetRectPoints().size(), drawColor, priority);
			DEBUGRENDERER2D->DrawString(m_Cells[cellIndex].boundingBox.bottomLeft + Elite::Vector2(0, m_Cells[cellIndex].boundingBox.height), std::to_string(m_Cells[cellIndex].agents.size()).c_str());
		}

		flock->RegisterNeighbors(pAgent);
		
		for (int neighborId{}; neighborId < flock->GetNrOfNeighbors(); ++neighborId)
		{
			SteeringAgent* neighbor = flock->GetNeighbors().at(neighborId); 
			DEBUGRENDERER2D->DrawCircle(neighbor->GetPosition(), neighbor->GetRadius(), Elite::Color(0, 1, 0), 0.01f);
		}

	}
	else
	{
		for (int cellIndex{}; cellIndex < m_NrOfCols * m_NrOfRows; ++cellIndex)
		{
			DEBUGRENDERER2D->DrawPolygon(m_Cells[cellIndex].GetRectPoints().data(), m_Cells[cellIndex].GetRectPoints().size(), Elite::Color(1, 0, 0), 0.1f);
			DEBUGRENDERER2D->DrawString(m_Cells[cellIndex].boundingBox.bottomLeft + Elite::Vector2(0, m_Cells[cellIndex].boundingBox.height), std::to_string(m_Cells[cellIndex].agents.size()).c_str());
		}
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2& pos) const
{
	const int row = (int) (std::clamp(pos.y, 0.0f, m_SpaceHeight - 0.01f) / m_CellHeight) ;
	const int column = (int) (std::clamp(pos.x, 0.0f, m_SpaceWidth - 0.01f) / m_CellWidth) ;
	return (row * m_NrOfCols + column);
}