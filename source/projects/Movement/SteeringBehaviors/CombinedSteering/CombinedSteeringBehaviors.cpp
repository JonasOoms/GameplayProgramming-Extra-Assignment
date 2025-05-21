#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"
   
BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& weightedBehaviors)
	:m_WeightedBehaviors(weightedBehaviors)
{
}

BlendedSteering::~BlendedSteering()
{
	for (WeightedBehavior& behavior : m_WeightedBehaviors)
	{
		SAFE_DELETE(behavior.pBehavior);
	}
}
;

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput blendedSteering = {};
	float totalWeight = 0.f;

	for (const BlendedSteering::WeightedBehavior behavior : m_WeightedBehaviors)
	{
		const SteeringOutput steering = behavior.pBehavior->CalculateSteering(deltaT, pAgent);
		blendedSteering.LinearVelocity += steering.LinearVelocity * behavior.weight;
		blendedSteering.AngularVelocity += steering.AngularVelocity * behavior.weight;
		totalWeight += behavior.weight;
	}

	if (totalWeight > 0.f)
	{
		blendedSteering /= totalWeight;
	}

	//TODO: Calculate the weighted average steeringbehavior

	if (pAgent->GetDebugRenderingEnabled())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), blendedSteering.LinearVelocity, 7, { 0, 1, 1 }, 0.40f);

	return blendedSteering;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		steering = pBehavior->CalculateSteering(deltaT, pAgent);

		if (steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return steering;
}