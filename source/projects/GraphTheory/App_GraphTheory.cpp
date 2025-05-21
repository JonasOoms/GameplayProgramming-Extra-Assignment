//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_GraphTheory.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EEularianPath.h"
#include "framework/EliteAI/EliteGraphs/EliteGraph/EGraphConnection.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

using namespace Elite;
using namespace std;

//Destructor
App_GraphTheory::~App_GraphTheory()
{
	SAFE_DELETE(m_pAgent);
	SAFE_DELETE(m_pPathFollowBehavior);
	SAFE_DELETE(m_pGraph);
	SAFE_DELETE(m_pGraphRenderer);
	SAFE_DELETE(m_pGraphEditor);
}

//Functions
void App_GraphTheory::Start()
{
	//Initialization of your application. If you want access to the physics world you will need to store it yourself.
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(80.f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(0, 0));
	DEBUGRENDERER2D->GetActiveCamera()->SetMoveLocked(false);
	DEBUGRENDERER2D->GetActiveCamera()->SetZoomLocked(false);

	m_pPathFollowBehavior = new PathFollow();
	m_pAgent = new SteeringAgent(2.f);
	m_pAgent->SetSteeringBehavior(m_pPathFollowBehavior);
	m_pAgent->SetMass(0.1f);
	m_pAgent->SetMaxLinearSpeed(20.f);
	m_pAgent->SetMaxAngularSpeed(90.f);
	m_pAgent->SetAutoOrient(true);

	m_pGraph = new Graph(false);
	int id0 = m_pGraph->AddNode(new GraphNode({ 20,30 }));
	int id1 = m_pGraph->AddNode(new GraphNode({ -10,-10 }));

	m_pGraph->AddConnection(new GraphConnection(id0, id1));

	m_pGraphRenderer = new GraphRenderer();
	m_pGraphEditor = new GraphEditor();


}

void App_GraphTheory::Update(float deltaTime)
{
	//------ INPUT ------

	if (m_pGraphEditor->UpdateGraph(m_pGraph))
	{
		m_pGraph->SetConnectionCostsToDistances();
		auto eulerFinder = Elite::EulerianPath(m_pGraph);
		Eulerianity isEuler = {};
		auto eulerTrail = eulerFinder.FindPath(isEuler);
		UpdateAgentPath(eulerTrail);
	}

	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 150;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
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

		ImGui::Text("Graph Theory");
		ImGui::Spacing();
		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif

	m_pAgent->Update(deltaTime);
}

void App_GraphTheory::Render(float deltaTime) const
{

	Elite::GraphRenderingOptions options(true, false, true, false);
	m_pGraphRenderer->RenderGraph(m_pGraph, options);

	m_pAgent->Render(deltaTime);

}

void App_GraphTheory::UpdateAgentPath(const vector<Elite::GraphNode*>& trail)
{
	vector<Vector2> path{};
	
	

	for (auto& node : trail)
	{
		path.emplace_back(node->GetPosition());
	}

	m_pPathFollowBehavior->SetPath(path);
	if (path.size() > 0)
	{
		m_pAgent->SetPosition(path[0]);
	}
}
