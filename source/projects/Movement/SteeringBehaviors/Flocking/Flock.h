#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class CellSpace;

class Flock final
{
public:
	Flock(
		int flockSize = 50, 
		float worldSize = 100.f, 
		SteeringAgent* const pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI() ;
	void Render(float deltaT);

	void RegisterNeighbors(SteeringAgent* const pAgent);
	int GetNrOfNeighbors() const;
	const std::vector<SteeringAgent*>& GetNeighbors() const;

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

	void SetTarget_Seek(const TargetData& target);
	void SetWorldTrimSize(float size) { m_WorldSize = size; }

private:
	//Datamembers
	int m_FlockSize = 0;
	std::vector<SteeringAgent*> m_Agents;
	std::vector<Elite::Vector2> m_OldPositions;
	std::vector<SteeringAgent*> m_Neighbors;

	bool m_UsePartitioning{ true };

	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;

	bool m_DrawCells;
	bool m_DrawAgentNeighbors;

	float m_NeighborhoodRadius = 50.f;
	int m_NrOfNeighbors = 0;

	SteeringAgent* m_pAgentToEvade = nullptr;
	
	//Steering Behaviors
	Separation* m_pSeparationBehavior = nullptr;
	Cohesion* m_pCohesionBehavior = nullptr;
	VelocityMatch* m_pVelMatchBehavior = nullptr;
	Seek* m_pSeekBehavior = nullptr;
	Wander* m_pWanderBehavior = nullptr;
	Evade* m_pEvadeBehavior = nullptr;

	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	CellSpace* m_pCellSpace = nullptr;

	void RenderNeighborhood();
	float* GetWeight(ISteeringBehavior* const pBehaviour);

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};