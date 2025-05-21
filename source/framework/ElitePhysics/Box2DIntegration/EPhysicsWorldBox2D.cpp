//=== General Includes ===
#include "stdafx.h"
#include "../Box2DIntegration/Box2DRenderer.h"

#ifdef USE_BOX2D
//=== Extra Information ===
struct Box2DPhysicsSettings final
{
	Box2DPhysicsSettings()
	{
		hz = 60.0f;
		velocityIterations = 8;
		positionIterations = 3;
	}

	float hz;
	int32 velocityIterations;
	int32 positionIterations;
};

//=== Constructors & Destructors ===
template<>
PhysicsWorld::~EPhysicsWorld()
{
	SAFE_DELETE(m_pPhysicsWorld);
	Box2DRenderer* pDebugRenderer = static_cast<Box2DRenderer*>(m_pDebugRenderer);
	SAFE_DELETE(pDebugRenderer);
}

//=== Functions ===
template <>
void PhysicsWorld::Initialize()
{
	//Create Physics
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	m_pPhysicsWorld = new b2World(gravity);
	m_pPhysicsWorld->SetAllowSleeping(true);
	m_pPhysicsWorld->SetWarmStarting(true);
	m_pPhysicsWorld->SetContinuousPhysics(true);
	m_pPhysicsWorld->SetSubStepping(false);

	Box2DRenderer* pDebugRenderer = new Box2DRenderer();
	const int flags = 1; //Render: shapes
	pDebugRenderer->SetFlags(flags);
	m_pPhysicsWorld->SetDebugDraw(pDebugRenderer);  //Link debugrenderer as debug drawer
	m_pDebugRenderer = pDebugRenderer; //Store pointer in void pointer to delete later
}

template<>
void PhysicsWorld::Simulate(float elapsedTime)
{
	if (!m_pPhysicsWorld)
		return;

	const Box2DPhysicsSettings physicsSettings;
	const float frameTime = physicsSettings.hz > 0.0f ? 1.0f / physicsSettings.hz : float(1.f / 60.f);

	if (elapsedTime > 0.25f)
		elapsedTime = 0.25f;

	m_FrameTimeAccumulator += elapsedTime;

	while (m_FrameTimeAccumulator >= frameTime)
	{
		m_pPhysicsWorld->Step(frameTime, physicsSettings.velocityIterations, physicsSettings.positionIterations);
		m_FrameTimeAccumulator -= frameTime;
	}
}

template<>
void PhysicsWorld::RenderDebug() const
{
	const Box2DRenderer* const pDebugRenderer = static_cast<Box2DRenderer*>(m_pDebugRenderer);
	if (pDebugRenderer != nullptr)
		m_pPhysicsWorld->DebugDraw();
}

template<>
std::vector<Elite::Polygon> PhysicsWorld::GetAllStaticShapesInWorld(PhysicsFlags userFlags) const
{
	//Locals
	std::vector<Elite::Polygon> vShapes = {};

	//Get all the shapes with certain flag, from static rigibodies
	for (b2Body* pB = m_pPhysicsWorld->GetBodyList(); pB; pB = pB->GetNext())
	{
		//Check if flagged
		if (pB->GetUserData() == nullptr)
			continue;
		//Retrieve data
		PhysicsFlags userData = *static_cast<PhysicsFlags*>(pB->GetUserData());
		if (((int)userData & (int)userFlags) == 0)
			continue;

		//Check based on type
		if (pB->GetType() == b2_staticBody)
		{
			//Get fixtures
			std::vector<b2Fixture*> vpFixtures;
			for (b2Fixture* f = pB->GetFixtureList(); f; f = f->GetNext())
				vpFixtures.push_back(f);

			//Foreach fixture
			for (b2Fixture* pF : vpFixtures)
			{
				std::vector<Vector2> points; //Temp container verts
				b2Shape::Type shapeType = pF->GetType();

				switch (shapeType)
				{
				case b2Shape::e_polygon:
				{
					b2PolygonShape* const pPolygonShape = static_cast<b2PolygonShape*>(pF->GetShape());
					//Get verts and store as polygon
					const int count = static_cast<int>(pPolygonShape->m_count);
					for (int i = 0; i < count; ++i)
					{
						b2Vec2 vert = pB->GetWorldPoint(pPolygonShape->m_vertices[i]);
						points.push_back(Vector2(vert.x, vert.y));
					}
					Polygon polygon = Polygon(points);
					polygon.OrientateWithChildren(Winding::CW);
					vShapes.push_back(polygon);
				}
				break;
				case b2Shape::e_circle:
				{
					//Clip out bounding box of circle, not the circle itself (too many "bad" triangles!)
					b2AABB aabb;
					aabb.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
					aabb.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
					b2CircleShape* const pCircleShape = static_cast<b2CircleShape*>(pF->GetShape());
					const int childCount = static_cast<int>(pCircleShape->GetChildCount());
					for (int i = 0; i < childCount; ++i)
						aabb.Combine(aabb, pF->GetAABB(i));

					b2Vec2 center = aabb.GetCenter();
					b2Vec2 extents = aabb.GetExtents();
					points.push_back(Vector2(center.x - extents.x, center.y - extents.y));
					points.push_back(Vector2(center.x - extents.x, center.y + extents.y));
					points.push_back(Vector2(center.x + extents.x, center.y + extents.y));
					points.push_back(Vector2(center.x + extents.x, center.y - extents.y));

					Polygon polygon = Polygon(points);
					polygon.OrientateWithChildren(Winding::CW);
					vShapes.push_back(polygon);
				}
				break;
				case b2Shape::e_edge: break; //Do nothing
				case b2Shape::e_chain: break; //Do nothing
				case b2Shape::e_typeCount: break; //Do nothing
				default: break;
				}
			}
		}
	}
	return vShapes;
}
#endif