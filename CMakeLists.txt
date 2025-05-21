# --- GENERAL CMake ---
cmake_minimum_required(VERSION 3.8)
project(GPP_Framework)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if Python ids available. This is required for the GL part
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(NOT Python3_FOUND)
    message(FATAL_ERROR "Python3 is required but not found. Please install Python3.")
endif()

# Fetch libraries
include(FetchContent)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.8
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.49
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(ImGui)

FetchContent_Declare(
    Box2D
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v2.4.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(Box2D)

FetchContent_Declare(
    gl3w
    GIT_REPOSITORY https://github.com/skaslev/gl3w.git
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(gl3w)

# We need to now execute the python script acquired from gl3w to pull in sources
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${gl3w_SOURCE_DIR}/gl3w_gen.py
    WORKING_DIRECTORY ${gl3w_SOURCE_DIR}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output)

if(result) # Non-zero exit code indicates failure
    message(FATAL_ERROR "Python script failed with error: ${error_output}")
else()
    message(STATUS "Python script executed successfully: ${output}")
endif()

# Create library for gl3w as this is required
add_library(gl3w_local STATIC "${gl3w_SOURCE_DIR}/src/gl3w.c")
target_include_directories(gl3w_local PUBLIC "${gl3w_SOURCE_DIR}/include/")

# Make sure OpenGL is available
find_package(OpenGL REQUIRED)

# Get all source files
set(SRC_PATH "source/")
set(FRAMEWORK_SRC_PATH "${SRC_PATH}/framework/")
set(PROJECTS_SRC_PATH "${SRC_PATH}/projects/")

set(IMGUI_SRC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp")

set(FRAMEWORK_SRC
    "${FRAMEWORK_SRC_PATH}/main.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteGeometry/EGeometry.h"
    "${FRAMEWORK_SRC_PATH}/EliteGeometry/EGeometry2DTypes.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteGeometry/EGeometry2DTypes.h"
    "${FRAMEWORK_SRC_PATH}/EliteGeometry/EGeometry2DUtilities.h"
    "${FRAMEWORK_SRC_PATH}/EliteHelpers/ESingleton.h"
    "${FRAMEWORK_SRC_PATH}/EliteInput/EInputCodes.h"
    "${FRAMEWORK_SRC_PATH}/EliteInput/EInputData.h"
    "${FRAMEWORK_SRC_PATH}/EliteInput/EInputManager.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteInput/EInputManager.h"
    "${FRAMEWORK_SRC_PATH}/EliteInterfaces/EIApp.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EMat22.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EMath.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EMathUtilities.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EMatrix2x3.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EMatrix2x3.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EVector2.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/EVector3.h"
    "${FRAMEWORK_SRC_PATH}/EliteMath/FMatrix.h"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/Box2DIntegration/Box2DRenderer.h"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/Box2DIntegration/EPhysicsWorldBox2D.cpp"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/Box2DIntegration/ERigidBodyBox2D.cpp"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/EPhysics.h"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/EPhysicsShapes.h"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/EPhysicsTypes.h"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/EPhysicsWorldBase.h"
    "${FRAMEWORK_SRC_PATH}/ElitePhysics/ERigidBodyBase.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/2DCamera/ECamera2D.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/2DCamera/ECamera2D.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/SDLIntegration/SDLDebugRenderer2D/SDLDebugRenderer2D.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/SDLIntegration/SDLDebugRenderer2D/SDLDebugRenderer2D.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/SDLIntegration/SDLFrame/SDLFrame.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/SDLIntegration/SDLFrame/SDLFrame.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/EDebugRenderer2D.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/EFrameBase.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/ERendering.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/ERenderingTypes.h"
    "${FRAMEWORK_SRC_PATH}/EliteRendering/Shaders.h"
    "${FRAMEWORK_SRC_PATH}/EliteTimer/SDLTimer/ETimer_SDL.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteTimer/ETimer.h"
    "${FRAMEWORK_SRC_PATH}/EliteUI/EImmediateUI.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteUI/EImmediateUI.h"
    "${FRAMEWORK_SRC_PATH}/EliteWindow/SDLWindow/SDLWindow.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteWindow/SDLWindow/SDLWindow.h"
    "${FRAMEWORK_SRC_PATH}/EliteWindow/EWindow.h"
    "${FRAMEWORK_SRC_PATH}/EliteWindow/EWindowBase.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraph/EGraph.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraph/EGraph.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraph/EGraphConnection.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraph/EGraphEnums.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraph/EGraphNode.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraph/EGraphNode.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EEularianPath.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphNodeFactory/EGraphNodeFactory.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphVisuals.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EBFS.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EBFS.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EHeuristic.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EHeuristicFunctions.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGridGraph/EConnectionCostCalculator.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGridGraph/EGridGraph.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGridGraph/EGridGraph.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainCostCalculator.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGraphNode.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGraphNode.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGridGraph.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGridGraph.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/ENavGraphPathfinding.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/ENavGraphPathfinding.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteGraphAlgorithms/EPathSmoothing.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteGraphs/EliteNavGraph/ENavGraphNode.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteData/EBlackboard.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteDecisionMaking/EliteFiniteStateMachine/EFiniteStateMachine.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteDecisionMaking/EliteFiniteStateMachine/EFiniteStateMachine.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteDecisionMaking/EDecisionMaking.h"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.cpp"
    "${FRAMEWORK_SRC_PATH}/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h")

set(PROJECT_SRC_BASE
    "${PROJECTS_SRC_PATH}/App_Selector.h"
    "${PROJECTS_SRC_PATH}/Shared/BaseAgent.cpp"
    "${PROJECTS_SRC_PATH}/Shared/BaseAgent.h"
    "${PROJECTS_SRC_PATH}/Movement/Sandbox/App_Sandbox.cpp"
    "${PROJECTS_SRC_PATH}/Movement/Sandbox/App_Sandbox.h"
    "${PROJECTS_SRC_PATH}/Movement/Sandbox/SandboxAgent.cpp"
    "${PROJECTS_SRC_PATH}/Movement/Sandbox/SandboxAgent.h"
    "${PROJECTS_SRC_PATH}/Shared/NavigationColliderElement.cpp"
    "${PROJECTS_SRC_PATH}/Shared/NavigationColliderElement.h")

set(PROJECT_SRC_W1_STEERING
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Steering/App_SteeringBehaviors.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Steering/App_SteeringBehaviors.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Steering/SteeringBehaviors.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Obstacle.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Obstacle.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/SteeringAgent.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/SteeringAgent.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/SteeringHelpers.h")

set(PROJECT_SRC_W2_COMBINED_STEERING
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/CombinedSteering/App_CombinedSteering.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/CombinedSteering/App_CombinedSteering.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Flocking/App_Flocking.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Flocking/App_Flocking.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Flocking/Flock.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Flocking/Flock.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Flocking/FlockingSteeringBehaviors.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/Flocking/FlockingSteeringBehaviors.h")

set(PROJECT_SRC_W3_SPACE_PARTITIONING
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.cpp"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h")

set(PROJECT_SRC_W4_GRAPH_THEORY
    "${PROJECTS_SRC_PATH}/GraphTheory/App_GraphTheory.cpp"
    "${PROJECTS_SRC_PATH}/GraphTheory/App_GraphTheory.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
    "${PROJECTS_SRC_PATH}/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.cpp")

set(PROJECT_SRC_W5_BFS_ASTAR
    "${PROJECTS_SRC_PATH}/Movement/Pathfinding/PathfindingAStar/App_PathfindingAStar.cpp"
    "${PROJECTS_SRC_PATH}/Movement/Pathfinding/PathfindingAStar/App_PathfindingAStar.h")

set(PROJECT_SRC_W6_NAV_MESH
    "${PROJECTS_SRC_PATH}/Movement/Pathfinding/NavMeshGraph/App_NavMeshGraph.cpp"
    "${PROJECTS_SRC_PATH}/Movement/Pathfinding/NavMeshGraph/App_NavMeshGraph.h")

set(PROJECT_SRC_W7_FSM
    "${PROJECTS_SRC_PATH}/DecisionMaking/FiniteStateMachines/App_FSM.cpp"
    "${PROJECTS_SRC_PATH}/DecisionMaking/FiniteStateMachines/App_FSM.h"
    "${PROJECTS_SRC_PATH}/DecisionMaking/FiniteStateMachines/StatesAndTransitions.cpp"
    "${PROJECTS_SRC_PATH}/DecisionMaking/FiniteStateMachines/StatesAndTransitions.h"
    "${PROJECTS_SRC_PATH}/DecisionMaking/SmartAgent.cpp"
    "${PROJECTS_SRC_PATH}/DecisionMaking/SmartAgent.h")

set(PROJECT_SRC_W8_BEHAVIOR_TREE
    "${PROJECTS_SRC_PATH}/DecisionMaking/BehaviorTrees/App_BT.cpp"
    "${PROJECTS_SRC_PATH}/DecisionMaking/BehaviorTrees/App_BT.h"
    "${PROJECTS_SRC_PATH}/DecisionMaking/BehaviorTrees/Behaviors.h")

# Create project
add_executable(${PROJECT_NAME} 
    ${IMGUI_SRC} ${FRAMEWORK_SRC} ${PROJECT_SRC_BASE}
    ${PROJECT_SRC_W1_STEERING}
    ${PROJECT_SRC_W2_COMBINED_STEERING}
    ${PROJECT_SRC_W3_SPACE_PARTITIONING}
    ${PROJECT_SRC_W4_GRAPH_THEORY}
    ${PROJECT_SRC_W5_BFS_ASTAR}
    ${PROJECT_SRC_W6_NAV_MESH}
    ${PROJECT_SRC_W7_FSM}
    ${PROJECT_SRC_W8_BEHAVIOR_TREE})

# Include and link libraries
target_include_directories(${PROJECT_NAME}
    PRIVATE ${SRC_PATH} ${imgui_SOURCE_DIR} "${box2d_SOURCE_DIR}/include/")

target_link_libraries(${PROJECT_NAME} 
    PRIVATE SDL2 gl3w_local OpenGL::GL box2d)

# Set precompiled headers
target_precompile_headers(${PROJECT_NAME} 
    PRIVATE "${SRC_PATH}/stdafx.h")

# Set output build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIG>")

set_target_properties(${PROJECT_NAME} SDL2 box2d gl3w_local
	PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})