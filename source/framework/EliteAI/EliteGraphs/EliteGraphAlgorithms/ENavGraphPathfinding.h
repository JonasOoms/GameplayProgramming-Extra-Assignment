#pragma once
#include <vector>
#include <iostream>
#include "framework/EliteMath/EMath.h"
#include "EPathSmoothing.h"


namespace Elite
{
	class NavGraph;


	class NavMeshPathfinding
	{
	public:
		static std::vector<Vector2> FindPath(const Vector2& startPos, const Vector2& endPos, NavGraph* const pNavGraph, 
			std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals);
		static std::vector<Vector2> FindPath(const Vector2& startPos, const Vector2& endPos, NavGraph* const pNavGraph);
	};
}
