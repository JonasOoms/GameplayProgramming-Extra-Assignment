//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "SteeringBehaviors.h"
#include "../Obstacle.h"

using namespace Elite;

//Destructor
App_SteeringBehaviors::~App_SteeringBehaviors()
{
	for (ImGui_Agent& imGuiAgent : m_AgentVec)
	{
		SAFE_DELETE(imGuiAgent.pAgent);
		SAFE_DELETE(imGuiAgent.pBehavior);
	}
	m_AgentVec.clear();

	for (Obstacle* pObstacle : m_Obstacles)
		SAFE_DELETE(pObstacle);
	m_Obstacles.clear();
}

void App_SteeringBehaviors::RemoveAgent(unsigned int index)
{
	SAFE_DELETE(m_AgentVec[index].pAgent);
	SAFE_DELETE(m_AgentVec[index].pBehavior);

	m_AgentVec.erase(m_AgentVec.begin() + index);
	m_TargetLabelsVec.clear();

	std::stringstream ss;
	m_TargetLabelsVec.push_back("Mouse");
	for (UINT i = 0; i < m_AgentVec.size(); ++i)
	{
		ss << "Agent " << i;
		m_TargetLabelsVec.push_back(ss.str());
		ss.str("");

		if (i >= index)
		{
			auto& agent = m_AgentVec[i];
			if (agent.SelectedTarget == index)
			{
				--agent.SelectedTarget;
			}
		}
	}
}

App_SteeringBehaviors::ImGui_Agent App_SteeringBehaviors::AddAgent(BehaviorTypes behaviorType, int targetId, bool autoOrient, float mass, float maxSpd)
{
	ImGui_Agent agent = {};
	agent.pAgent = new SteeringAgent();
	agent.pAgent->SetAutoOrient(autoOrient);
	agent.pAgent->SetMaxLinearSpeed(maxSpd);
	agent.pAgent->SetMass(mass);
	agent.pAgent->SetPosition({ RandomFloat(m_TrimWorldSize) / 2, RandomFloat(m_TrimWorldSize) / 2 });

	agent.SelectedBehavior = int(behaviorType);
	agent.SelectedTarget = targetId;

	if (m_IsInitialized)
		SetAgentBehavior(agent);

	m_AgentVec.push_back(agent);

	if (m_IsInitialized)
		UpdateTargetLabel();

	return agent;
}

//Functions
void App_SteeringBehaviors::Start()
{
	//AddObstacle();
	//AddObstacle();
	//AddObstacle();
	//AddObstacle();

	AddAgent(BehaviorTypes::Seek, -1);
	m_AgentVec[0].pAgent->SetDebugRenderingEnabled(true);

	std::stringstream ss;
	m_TargetLabelsVec.push_back("Mouse");
	for (UINT i = 0; i < m_AgentVec.size(); ++i)
	{
		ss << "Agent " << i;
		m_TargetLabelsVec.push_back(ss.str());
		ss.str("");

		SetAgentBehavior(m_AgentVec[i]);
	}

	m_IsInitialized = true;
}

void App_SteeringBehaviors::Update(float deltaTime)
{
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft) && m_VisualizeTarget)
	{
		const Elite::MouseData& mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_Target.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
		for (ImGui_Agent& a : m_AgentVec)
		{
			UpdateTarget(a);
		}
	}

#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int const menuWidth = 235;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Steering Behaviors");
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox("Trim World", &m_TrimWorld);
		if (m_TrimWorld)
		{
			ImGui::SliderFloat("Trim Size", &m_TrimWorldSize, 0.f, 200.f, "%.1");
		}
		ImGui::Spacing();

		if (ImGui::Button("Add Agent"))
			AddAgent(BehaviorTypes::Seek);

		if (ImGui::Button("Add Obstacle"))
			AddObstacle();

		ImGui::Spacing();
		ImGui::Separator();

		for (UINT i = 0; i < m_AgentVec.size(); ++i)
		{
			ImGui::PushID(i);
			char headerName[100];
			snprintf(headerName, sizeof(headerName), "ACTOR %i", i);

			ImGui_Agent& a = m_AgentVec[i];

			if (ImGui::CollapsingHeader(headerName))
			{
				ImGui::Indent();
				//Actor Props
				if (ImGui::CollapsingHeader("Properties"))
				{
					float v = a.pAgent->GetMaxLinearSpeed();
					if (ImGui::SliderFloat("Lin", &v, 0.f, 20.f, "%.2f"))
						a.pAgent->SetMaxLinearSpeed(v);

					v = a.pAgent->GetMaxAngularSpeed();
					if (ImGui::SliderFloat("Ang", &v, 0.f, 20.f, "%.2f"))
						a.pAgent->SetMaxAngularSpeed(v);

					v = a.pAgent->GetMass();
					if (ImGui::SliderFloat("Mass ", &v, 0.f, 20.f, "%.2f"))
						a.pAgent->SetMass(v);
				}


				bool behaviourModified = false;

				ImGui::Spacing();

				ImGui::PushID(i + 50);
				ImGui::AlignFirstTextHeightToWidgets();
				ImGui::Text(" Behavior: ");
				ImGui::SameLine();
				ImGui::PushItemWidth(100);
				//TODO: ADD Extra steering behaviors here, separated by "\0". Order must be the same as in the BehaviorTypes enum.
				if (ImGui::Combo("", &a.SelectedBehavior, "Seek\0Wander\0Arrive\0Flee\0Face\0Pursuit", 4))
				{
					behaviourModified = true;
				}
				ImGui::PopItemWidth();
				ImGui::PopID();

				ImGui::Spacing();
				ImGui::PushID(i + 100);
				ImGui::AlignFirstTextHeightToWidgets();
				ImGui::Text(" Target: ");
				ImGui::SameLine();
				int itemSelected2 = 0;
				ImGui::PushItemWidth(100);
				int selectedTargetOffset = a.SelectedTarget + 1;
				if (ImGui::Combo("", &selectedTargetOffset, [](void* vec, int idx, const char** out_text)
					{
						std::vector<std::string>* vector = reinterpret_cast<std::vector<std::string>*>(vec);

						if (idx < 0 || idx >= (int)vector->size())
							return false;

						*out_text = vector->at(idx).c_str();
						return true;
					}, reinterpret_cast<void*>(&m_TargetLabelsVec), static_cast<int>(m_TargetLabelsVec.size())))
				{
					a.SelectedTarget = selectedTargetOffset - 1;
					behaviourModified = true;
				}
				ImGui::PopItemWidth();
				ImGui::PopID();
				ImGui::Spacing();
				ImGui::Spacing();

				if (behaviourModified)
					SetAgentBehavior(a);

				if (ImGui::Button("x"))
				{
					m_AgentToRemove = i;
				}

				ImGui::SameLine(0, 20);

				bool isChecked = a.pAgent->GetDebugRenderingEnabled();
				if (ImGui::Checkbox("Debug Rendering", &isChecked))
				{
					a.pAgent->SetDebugRenderingEnabled(isChecked);
				}

				ImGui::Unindent();
			}

			ImGui::PopID();
		}

		if (m_AgentToRemove >= 0)
		{
			RemoveAgent(m_AgentToRemove);
			m_AgentToRemove = -1;
		}

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif

	for (ImGui_Agent& a : m_AgentVec)
	{
		if (a.pAgent)
		{
			a.pAgent->Update(deltaTime);

			if (m_TrimWorld)
				a.pAgent->TrimToWorld(m_TrimWorldSize);

			UpdateTarget(a);
		}
	}

	float rot = m_AgentVec[0].pAgent->GetRotation();
	Vector2 p1 = Vector2{ 0,0 };
	Vector2 p2 = Vector2{ cos(rot), sin(rot) };
	DEBUGRENDERER2D->DrawSegment(p1, p2, { 1,0,0 });
}

void App_SteeringBehaviors::Render(float deltaTime) const
{
	for (const ImGui_Agent& a : m_AgentVec)
	{
		if (a.pAgent)
		{
			a.pAgent->Render(deltaTime);
		}
	}

	if (m_TrimWorld)
	{
		RenderWorldBounds(m_TrimWorldSize);
	}

	//Render Target
	if (m_VisualizeTarget)
		DEBUGRENDERER2D->DrawSolidCircle(m_Target.Position, 0.3f, { 0.f,0.f }, { 1.f,0.f,0.f }, -0.8f);
}

void App_SteeringBehaviors::SetAgentBehavior(ImGui_Agent& a)
{
	SAFE_DELETE(a.pBehavior);
	bool const useMouseAsTarget = a.SelectedTarget < 0;
	bool autoOrient = true;

	switch (BehaviorTypes(a.SelectedBehavior))
	{
	default:
	case BehaviorTypes::Seek:
		a.pBehavior = new Seek();
		break;
	case BehaviorTypes::Arrive:
			a.pBehavior = new Arrive();
			break;
	case BehaviorTypes::Wander:
		a.pBehavior = new Wander();
		break;
	case BehaviorTypes::Flee:
		a.pBehavior = new Flee();
		break;
	case BehaviorTypes::Face:
		autoOrient = false;
		a.pBehavior = new Face();
		break;
	case BehaviorTypes::Pursuit:
		a.pBehavior = new Pursuit();
		break;
	case BehaviorTypes::Evade:
		a.pBehavior = new Evade(15.f);
		break;
	//TODO: handle other steering behaviours here
	}

	UpdateTarget(a);

	a.pAgent->SetAutoOrient(autoOrient);
	a.pAgent->SetSteeringBehavior(a.pBehavior);
}

void App_SteeringBehaviors::UpdateTarget(ImGui_Agent& a)
{
	bool const useMouseAsTarget = a.SelectedTarget < 0;
	if (useMouseAsTarget)
		a.pBehavior->SetTarget(m_Target);
	else
	{
		SteeringAgent* const pAgent = m_AgentVec[a.SelectedTarget].pAgent;
		TargetData target = TargetData{};
		target.Position = pAgent->GetPosition();
		target.Orientation = pAgent->GetRotation();
		target.LinearVelocity = pAgent->GetLinearVelocity();
		target.AngularVelocity = pAgent->GetAngularVelocity();
		a.pBehavior->SetTarget(target);
	}
}

void App_SteeringBehaviors::UpdateTargetLabel()
{
	m_TargetLabelsVec.clear();

	std::stringstream ss;
	m_TargetLabelsVec.push_back("Mouse");
	int const count = static_cast<int>(m_AgentVec.size());
	for (int i = 0; i < count; ++i)
	{
		ss << "Agent " << i;
		m_TargetLabelsVec.push_back(ss.str());
		ss.str("");
	}
}

void App_SteeringBehaviors::AddObstacle()
{
	float const radius = RandomFloat(m_MinObstacleRadius, m_MaxObstacleRadius);
	bool positionFound = false;
	Vector2 const pos = GetRandomObstaclePosition(radius, positionFound);

	if (positionFound)
		m_Obstacles.push_back(new Obstacle(pos, radius));
}

Elite::Vector2 App_SteeringBehaviors::GetRandomObstaclePosition(float newRadius, bool& positionFound)
{
	positionFound = false;
	Elite::Vector2 pos;
	int tries = 0;
	int maxTries = 200;

	while (positionFound == false && tries < maxTries)
	{
		positionFound = true;
		pos = RandomVector2(0, m_TrimWorldSize);

		for (const Obstacle* pObstacle : m_Obstacles)
		{
			float const radiusSum = newRadius + pObstacle->GetRadius();
			float const distance = Distance(pos, pObstacle->GetCenter());

			if (distance < radiusSum + m_MinObstacleDistance)
			{
				positionFound = false;
				break;
			}
		}
		++tries;
	}

	return pos;
}
