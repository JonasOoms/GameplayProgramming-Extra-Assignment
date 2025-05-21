/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
// BT TODO:

namespace BT_Actions 
{
	Elite::BehaviorState Patrol(Elite::Blackboard* const pBlackboard)
	{
		SmartAgent* pAgent{ nullptr };
		std::vector<Elite::Vector2> patrolPath;
		PathFollow* pPathFollow;

		pBlackboard->GetData("Agent", pAgent);
		pBlackboard->GetData("PatrolPath", patrolPath);
		pBlackboard->GetData("PathFollow", pPathFollow);

		assert(pAgent && "Agent not set in blackboard!");
		assert(pPathFollow && "PathFollow not set in blackboard!");
		assert(!patrolPath.empty() && "Patrol path is empty!");

		pAgent->SetSteeringBehavior(pPathFollow);
		if (pPathFollow->HasArrived())
		{
			pPathFollow->SetPath(patrolPath);
		}
		return Elite::BehaviorState::Success;
	}
}

namespace BT_Conditions
{
	bool IsTargetVisible(Elite::Blackboard* const pBlackboard)
	{
		SmartAgent* pAgent{ nullptr };
		SteeringAgent* pPlayer{ nullptr };
		float detectionRadius;

		pBlackboard->GetData("Agent", pAgent);
		pBlackboard->GetData("TargetAgent", pPlayer);
		pBlackboard->GetData("DetectionRadius", detectionRadius);

		assert(pAgent && "Agent not set in blackboard!");
		assert(pPlayer && "Player not set in blackboard!");
		assert(!isnan(detectionRadius) && "No detectionradius set!");

		const bool isInDetectionRadius = Elite::DistanceSquared(pAgent->GetPosition(), pPlayer->GetPosition()) < (detectionRadius * detectionRadius);
		const bool isInLineOfSight = pAgent->HasLineOfSight(pPlayer->GetPosition());

		return isInDetectionRadius && isInLineOfSight;


	}
}

#endif