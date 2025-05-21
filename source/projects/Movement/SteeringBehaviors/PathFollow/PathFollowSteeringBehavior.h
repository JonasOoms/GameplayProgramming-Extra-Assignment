#pragma once

#include "../Steering/SteeringBehaviors.h"

class PathFollow : public ISteeringBehavior
{
public:
	PathFollow();
	virtual ~PathFollow();
	void SetPath(std::vector<Elite::Vector2>& path);
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* const pAgent) override;
	bool HasArrived() const;

private:
	Seek* m_pSeek = nullptr;
	Arrive* m_pArrive = nullptr; // TODO: Arrive not part of start files initially, add the definition as well
	ISteeringBehavior* m_pCurrentSteering = nullptr;
	std::vector<Elite::Vector2> m_pathVec = {};
	int m_currentPathIndex = 0;

	void GotoNextPathpoint();
};