/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringAgent.h: basic agent using steering behaviors
/*=============================================================================*/
#ifndef STEERING_AGENT_H
#define STEERING_AGENT_H

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../../Shared/BaseAgent.h"
#include "SteeringHelpers.h"
class ISteeringBehavior;

class SteeringAgent : public BaseAgent
{
public:
	//--- Constructor & Destructor ---
	SteeringAgent() = default;
	SteeringAgent(float radius) : BaseAgent(radius) {};
	virtual ~SteeringAgent() = default;

	//--- Agent Functions ---
	void Update(float dt) override;
	void Render(float dt) override;

	float GetMaxLinearSpeed() const { return m_MaxLinearSpeed; }
	void SetMaxLinearSpeed(float maxLinSpeed) { m_MaxLinearSpeed = maxLinSpeed; }

	float GetMaxAngularSpeed() const { return m_MaxAngularSpeed; }
	void SetMaxAngularSpeed(float maxAngSpeed) { m_MaxAngularSpeed = maxAngSpeed; }

	bool IsAutoOrienting() const { return m_AutoOrient; }
	void SetAutoOrient(bool autoOrient) { m_AutoOrient = autoOrient; }

	Elite::Vector2 GetDirection() const { return GetLinearVelocity().GetNormalized(); }

	virtual void SetSteeringBehavior(ISteeringBehavior* const pBehavior) { m_pSteeringBehavior = pBehavior; }
	ISteeringBehavior* const GetSteeringBehavior() const { return m_pSteeringBehavior; }

	void SetDebugRenderingEnabled(bool isEnabled) { m_RenderDebug = isEnabled; }
	bool GetDebugRenderingEnabled() const { return m_RenderDebug; }

protected:
	//--- Datamembers ---
	ISteeringBehavior* m_pSteeringBehavior = nullptr;

	float m_MaxLinearSpeed = 10.f;
	float m_MaxAngularSpeed = 10.f;
	bool m_AutoOrient = false;
	bool m_RenderDebug = false;
};
#endif