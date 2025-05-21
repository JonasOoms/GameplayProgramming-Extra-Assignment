#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"
#include "../SpacePartitioning/SpacePartitioning.h"
using namespace Elite;
//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* const pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 5.f }
	, m_NrOfNeighbors{0}
{
	m_Agents.resize(m_FlockSize);
	m_OldPositions.resize(m_FlockSize);
	m_Neighbors.resize(m_FlockSize);


	m_pCellSpace = new CellSpace(m_WorldSize, m_WorldSize, 25, 25, m_FlockSize);

	m_pSeparationBehavior = new Separation(this);
	m_pCohesionBehavior = new Cohesion(this);
	m_pVelMatchBehavior = new VelocityMatch(this);
	m_pSeekBehavior = new Seek();
	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade(15);

	m_pAgentToEvade->SetSteeringBehavior(m_pSeekBehavior);
	m_pAgentToEvade->SetMaxLinearSpeed(20.f);
	m_pAgentToEvade->SetBodyColor(Color(1.f, 0.f, 0.f));

	m_pBlendedSteering = new BlendedSteering({ {m_pCohesionBehavior, 0.2f}, {m_pSeparationBehavior, 0.1f }, {m_pVelMatchBehavior, 0.6f}, {m_pSeekBehavior, .2f}, {m_pWanderBehavior, 1.f} });
	m_pPrioritySteering = new PrioritySteering({m_pEvadeBehavior, m_pBlendedSteering});

	for (int i{}; i < flockSize; ++i)
	{
		m_Agents[i] = new SteeringAgent();
		m_Agents[i]->SetAutoOrient(true);
		m_Agents[i]->SetMaxLinearSpeed(10.f);
		m_Agents[i]->SetMass(1.f);
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);

		m_Agents[i]->SetPosition({ Elite::RandomFloat(0.f, worldSize), Elite::RandomFloat(0.f, worldSize) });
		
		m_pCellSpace->AddAgent(m_Agents[i]);
	}


}

Flock::~Flock()
{


	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pCellSpace)

	for(SteeringAgent* pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();
	m_Neighbors.clear();
}

void Flock::Update(float deltaT)
{
	

	for (int agentIndex{}; agentIndex < m_Agents.size(); ++agentIndex)
	{
		SteeringAgent* agent = m_Agents[agentIndex];
		RegisterNeighbors(agent);
		agent->Update(deltaT);
		if (m_TrimWorld)
		{
			agent->TrimToWorld(m_WorldSize);
		}
		m_pCellSpace->AgentPositionChanged(agent, m_OldPositions[agentIndex]);
		m_OldPositions[agentIndex] = agent->GetPosition();
	}
	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld(m_WorldSize);

	TargetData target{};
	target.Position = m_pAgentToEvade->GetPosition();
	target.LinearVelocity = m_pAgentToEvade->GetLinearVelocity();
	target.AngularVelocity = m_pAgentToEvade->GetAngularVelocity();

	m_pEvadeBehavior->SetTarget(target);
}

void Flock::Render(float deltaT)
{
	
	/*for (SteeringAgent* pAgent : m_Agents)
	{
		pAgent->Render(deltaT);
	}*/
	

	if (true) 
		RenderNeighborhood();
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
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

	ImGui::Text("Flocking");
	ImGui::Spacing();

	ImGui::Checkbox("Use Spatial Partitioning", &m_UsePartitioning);
	if (m_UsePartitioning)
	{
		ImGui::Indent();
		ImGui::Checkbox("Draw Cells", &m_DrawCells);
		ImGui::Checkbox("Draw Agent Neighbors", &m_DrawAgentNeighbors);
		ImGui::Unindent();
	}


	ImGui::Text("Behavior Weights");
	ImGui::Spacing();


	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Velocity Match", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");
	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RenderNeighborhood()
{
	if (m_UsePartitioning)
	{
		if (m_DrawCells)
		{
			m_pCellSpace->RenderCells(m_DrawAgentNeighbors, m_Agents[0], this);
		}
	}
	
}


void Flock::RegisterNeighbors(SteeringAgent* const pAgent)
{
	if (m_UsePartitioning)
	{
		m_pCellSpace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
	}
	else
	{
		for (int neighborIndex{}; neighborIndex < m_NrOfNeighbors; ++neighborIndex)
		{
			m_Neighbors[neighborIndex] = nullptr;
		}
		m_NrOfNeighbors = 0;

		for (const auto& agent : m_Agents)
		{
			if (agent == pAgent)
				continue;
			Elite::Vector2 agentPos = agent->GetPosition();
			Elite::Vector2 pAgentPos = pAgent->GetPosition();
			float distance = Elite::Distance(agentPos, pAgent->GetPosition());
			if (distance < m_NeighborhoodRadius)
			{
				m_Neighbors[m_NrOfNeighbors] = agent;
				++m_NrOfNeighbors;
			}
		}
	}
}

int Flock::GetNrOfNeighbors() const
{
	if (m_UsePartitioning)
	{
		return m_pCellSpace->GetNrOfNeighbors();
	}
	return m_NrOfNeighbors;
}

const std::vector<SteeringAgent*>& Flock::GetNeighbors() const
{
	if (m_UsePartitioning)
	{
		return m_pCellSpace->GetNeighbors();
	}
	return m_Neighbors;
}

Vector2 Flock::GetAverageNeighborPos() const
{
	Vector2 avgPosition = Elite::ZeroVector2;

	
	for (int neighborIndex{}; neighborIndex < GetNrOfNeighbors(); ++neighborIndex)
	{
		avgPosition += GetNeighbors().at(neighborIndex)->GetPosition();
	}
	if (GetNrOfNeighbors() != 0)
	{
		avgPosition /= GetNrOfNeighbors();
	}
	
	return avgPosition;
}

Vector2 Flock::GetAverageNeighborVelocity() const
{
	Vector2 avgVelocity = Elite::ZeroVector2;
	
	for (int neighborIndex{}; neighborIndex < GetNrOfNeighbors(); ++neighborIndex)
	{
		avgVelocity += GetNeighbors().at(neighborIndex)->GetLinearVelocity();
	}
	if (GetNrOfNeighbors() != 0)
	{
		avgVelocity /= GetNrOfNeighbors();
	}
	

	return avgVelocity;
}
void Flock::SetTarget_Seek(const TargetData& target)
{
	m_pSeekBehavior->SetTarget(target);
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		std::vector<BlendedSteering::WeightedBehavior>& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](const BlendedSteering::WeightedBehavior& el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
