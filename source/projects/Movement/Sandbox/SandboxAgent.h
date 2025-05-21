#ifndef SANDBOX_AGENT_H
#define SANDBOX_AGENT_H
#include "projects/Shared/BaseAgent.h"

class SandboxAgent:public BaseAgent
{
public:
	SandboxAgent();
	~SandboxAgent() = default;

	//Functions
	void Update(float dt) override;
	void Render(float dt) override;

	void SetTarget(const Elite::Vector2& target) { m_Target = target; }
	Elite::Vector2 GetTarget() { return m_Target; }
	void SetCurrentBehaviour(int index);
	int GetCurrentBehaviour() { return m_CurrentBehavior; }

private:

	//Functions
	void AutoOrient();
	void KinematicSeekAndArrive() const;
	void KinematicWander() const;

	//Members
	Elite::Vector2 m_Target = {};
	int m_CurrentBehavior = 0;
};
#endif