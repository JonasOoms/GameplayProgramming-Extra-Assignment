#include "stdafx.h"
#include "SandboxAgent.h"

using namespace Elite;

SandboxAgent::SandboxAgent(): BaseAgent()
{
	m_Target = GetPosition();
}

void SandboxAgent::Update(float dt)
{

	switch (m_CurrentBehavior)
	{
	case 0:
		KinematicSeekAndArrive();
		break;
	case 1:
		KinematicWander();
		break;
	default:
		break;
	}

	//TODO: set linear velocity towards m_Target

	//Orientation
	AutoOrient();
}

void SandboxAgent::Render(float dt)
{
	BaseAgent::Render(dt); //Default Agent Rendering

	const Elite::Vector2 position = GetPosition();
	const Elite::Vector2 velocity = GetLinearVelocity();
	const Elite::Vector2 Direction = velocity.GetNormalized() * 1.5f;

	DEBUGRENDERER2D->DrawSegment(position, position + Direction, Color(0, 1, 0));
	DEBUGRENDERER2D->DrawString(position + Elite::Vector2(1.f, -1.f), "x:%.2f, y: %.2f", velocity.x, velocity.y);

}

void SandboxAgent::SetCurrentBehaviour(int index)
{
	m_CurrentBehavior = index;
	if (m_CurrentBehavior < 0) m_CurrentBehavior = 0;
	else if (m_CurrentBehavior) m_CurrentBehavior = 1;
}

void SandboxAgent::AutoOrient()
{
	//Determine angle based on direction
	Vector2 const velocity = GetLinearVelocity();
	if (velocity.MagnitudeSquared() > 0)
	{
		SetRotation(VectorToOrientation(velocity));
	}
}

void SandboxAgent::KinematicSeekAndArrive() const
{
	const float radius = 1.f; 
	const float maxSpeed = 100.f;
	const float timeToTarget = 0.25f;

	Elite::Vector2 direction = m_Target - GetPosition();
	if (direction.MagnitudeSquared() < (radius * radius))
	{
		SetLinearVelocity(Elite::ZeroVector2);
		return;
	}

	Elite::Vector2 velocity = Elite::Vector2(direction.x / timeToTarget,
		direction.y / timeToTarget);

	SetLinearVelocity(velocity);
	SetAngularVelocity(0.f);

}

void SandboxAgent::KinematicWander() const
{
	const float max = 2.f * static_cast<float>(E_PI);
	const float randomAngle = (max * static_cast<float>(rand()) / RAND_MAX)
	- (max * static_cast<float>(rand()) / RAND_MAX);
	const Elite::Vector2 angleVec = Elite::Vector2(sin(randomAngle), cos(randomAngle));

	const float distance = 5.f;
	const float speed = 5.f;

	Elite::Vector2 velocity = GetLinearVelocity();
	velocity.Normalize();
	velocity *= distance;
	velocity += angleVec;
	velocity.Normalize();
	velocity *= speed;


	SetLinearVelocity(velocity);
}
