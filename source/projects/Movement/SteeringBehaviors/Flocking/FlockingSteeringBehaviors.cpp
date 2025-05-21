#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
    SteeringOutput steering{};

    Elite::Vector2 avgPosition = m_pFlock->GetAverageNeighborPos();
	
    m_Target.Position = avgPosition;
	Seek::CalculateSteering(deltaT, pAgent);


  /*  steering.LinearVelocity = avgPosition - pAgent->GetPosition().GetNormalized();
    steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();*/

    return steering;
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
    SteeringOutput steering{};

    auto neighbours = m_pFlock->GetNeighbors();
    int nrOfNeighbors = m_pFlock->GetNrOfNeighbors();

    if (nrOfNeighbors > 0)
    {
        for (int neighborIndex{}; neighborIndex < m_pFlock->GetNrOfNeighbors(); ++neighborIndex)
        {


            const float distance = Elite::Distance(pAgent->GetPosition(), neighbours[neighborIndex]->GetPosition());
            const float inverseDistance = 1.0f / distance;
            TargetData targetData;
            targetData.Position = neighbours[neighborIndex]->GetPosition();
            Flee::SetTarget(targetData);
            steering.LinearVelocity += Flee::CalculateSteering(deltaT, pAgent).LinearVelocity * inverseDistance;

        }

      

        steering.LinearVelocity.Normalize();
        steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
    }

    return steering;
}


//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
    SteeringOutput steering{};

    steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity().GetNormalized();
    steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

    return steering;
}