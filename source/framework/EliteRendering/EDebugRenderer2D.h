/*=============================================================================*/
// Copyright 2021-2022
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================
DebugRenderer2D.h: OpenGL immediate 2D debug renderer, used for drawing 2D shapes.
=============================================================================*/
#ifndef ELITE_DEBUG_RENDERER_2D_H
#define ELITE_DEBUG_RENDERER_2D_H
//--- Includes ---
#include "2DCamera/ECamera2D.h"
#include "ERenderingTypes.h"
#include "../EliteGeometry/EGeometry2DTypes.h"
namespace Elite
{
	template<typename Impl>
	class EDebugRenderer2D
	{
	public:
		//--- Constructor & Destructor ---
		EDebugRenderer2D() = default;
		~EDebugRenderer2D() = default;

		//--- Functions ---
		void Initialize(Camera2D* const pActiveCamera);
		void Render();
		unsigned int LoadShadersToProgram(const char* const vertexShaderPath, const char* const fragmentShaderPath);
		unsigned int LoadShadersToProgramFromEmbeddedSource(const char* const vertexShader, const char* const fragmentShader);
		Camera2D* GetActiveCamera() const { return m_pActiveCamera; }

		//--- User Functions ---
		void DrawPolygon(Elite::Polygon* const polygon, const Elite::Color& color, float depth);
		void DrawPolygon(Elite::Polygon* const polygon, const Elite::Color& color);
		void DrawPolygon(const Elite::Vector2* const points, int count, const Elite::Color& color, float depth);
		void DrawSolidPolygon(Elite::Polygon* const polygon, const Elite::Color& color, float depth, bool triangulate = false);
		void DrawSolidPolygon(const Elite::Polygon* const polygon, const Elite::Color& color, float depth);
		void DrawSolidPolygon(const Elite::Vector2* const points, int count, const Elite::Color& color, float depth, bool triangulate = false);
		void DrawCircle(const Elite::Vector2& center, float radius, const Elite::Color& color, float depth);
		void DrawSolidCircle(const Elite::Vector2& center, float radius, const Elite::Vector2& axis, const Elite::Color& color, float depth);
		void DrawSolidCircle(const Elite::Vector2& center, float radius, const Elite::Vector2& axis, const Elite::Color& color);
		void DrawSegment(const Elite::Vector2& p1, const Elite::Vector2& p2, const Elite::Color& color, float depth);
		void DrawSegment(const Elite::Vector2& p1, const Elite::Vector2& p2, const Elite::Color& color);
		void DrawDirection(const Elite::Vector2& p, const Elite::Vector2& dir, float length, const Elite::Color& color, float depth = 0.9f);
		void DrawTransform(const Elite::Vector2& p, const Elite::Vector2& xAxis, const Elite::Vector2& yAxis, float depth);
		void DrawPoint(const Elite::Vector2& p, float size, const Elite::Color& color, float depth = 0.9f);
		void DrawString(int x, int y, const char* const string, ...) const;
		void DrawString(const Elite::Vector2& pw, const char* const string, ...) const;

		inline float NextDepthSlice();

	protected:
		//General
		Camera2D* m_pActiveCamera = nullptr;
		float m_CurrDepthSlice = 0.f;

		//Containers of different INTERLEAVED primitiveTypes
		std::vector<Vertex> m_vPoints;
		std::vector<Vertex> m_vLines;
		std::vector<Vertex> m_vTriangles;

		//Functions
		void Shutdown();
	};
}
#endif