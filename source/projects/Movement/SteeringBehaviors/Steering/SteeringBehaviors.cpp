//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"
#include <limits>

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color(0, 1, 0), 0.4f);
	}

	return steering;
}

//ARRIVE
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();

	const float distance = steering.LinearVelocity.Normalize() - m_TargetRadius;

	if (distance < m_SlowRadius)
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * (distance / (m_SlowRadius + m_TargetRadius));
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}

	if (pAgent->GetDebugRenderingEnabled())
	{
		const Elite::Vector2 pos = pAgent->GetPosition();
		DEBUGRENDERER2D->DrawCircle(pos, m_SlowRadius, Elite::Color(0, 1, 0.5f), 0.7f);
		DEBUGRENDERER2D->DrawCircle(pos, m_TargetRadius, Elite::Color(1.f, 0.25f, 0.0f), 0.7f);
		DEBUGRENDERER2D->DrawDirection(pos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color(0.f, 1.f, 0.f, 0.5f), 0.7f);
	}

	return steering;
}

SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* const pAgent )
{

	SteeringOutput steering = {};

	Elite::Vector2 offsetPoint = pAgent->GetPosition() + pAgent->GetLinearVelocity().GetNormalized() * m_OffsetDistance;
	const float randomAngle = m_WanderAngle + Elite::RandomFloat(-m_MaxAngleChange, m_MaxAngleChange);
	Elite::Vector2 randomPoint = offsetPoint + Elite::Vector2(m_Radius * cosf(randomAngle), m_Radius * sinf(randomAngle));
	m_WanderAngle = randomAngle;

	steering.LinearVelocity = randomPoint - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	return steering;

}

SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = -(m_Target.Position - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color(0, 1, 0), 0.4f);
	}

	return steering;
}

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};


	float desiredAngle = atan2(m_Target.Position.y - pAgent->GetPosition().y, m_Target.Position.x - pAgent->GetPosition().x);


	float angleDifference = desiredAngle - pAgent->GetRotation();


	while (angleDifference > M_PI) angleDifference -= 2 * M_PI;
	while (angleDifference < -M_PI) angleDifference += 2 * M_PI;


	float maxAngularSpeed = pAgent->GetMaxAngularSpeed();
	float angularSpeed = angleDifference * maxAngularSpeed;


	steering.AngularVelocity = std::clamp(angularSpeed, -maxAngularSpeed, maxAngularSpeed);

	return steering;



}


SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};

	float distance = (m_Target.Position - pAgent->GetPosition()).Magnitude();
	float time = (distance / m_Target.LinearVelocity.Magnitude());
	Elite::Vector2 PredictedPosition = m_Target.Position + m_Target.LinearVelocity * time;

	steering.LinearVelocity = PredictedPosition - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color(0, 1, 0), 0.4f);
	}

	return steering;

}

Evade::Evade(float radius):
	ISteeringBehavior(),
	m_Radius(radius)
{
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* const pAgent)
{
	SteeringOutput steering = {};

	float distance = (m_Target.Position - pAgent->GetPosition()).Magnitude();

	if (distance > m_Radius)
	{
		steering.IsValid = false;
		return steering;
	}

	float time = (distance / m_Target.LinearVelocity.Magnitude());
	Elite::Vector2 PredictedPosition = m_Target.Position + m_Target.LinearVelocity * time;

	steering.LinearVelocity = -PredictedPosition + pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color(0, 1, 0), 0.4f);
	}
	return steering;
}