/*=============================================================================*/
// Copyright 2021-2022
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================
DebugRenderer2D.h: OpenGL immediate 2D debug renderer, used for drawing 2D shapes.
=============================================================================*/
#ifndef ELITE_SDL_RENDERER_2D_H
#define ELITE_SDL_RENDERER_2D_H

//--- Includes ---
#include "../../EDebugRenderer2D.h"
#include "../../ERenderingTypes.h"
#include "../../../EliteGeometry/EGeometry2DTypes.h"
#include "../../Shaders.h"

namespace Elite
{
	class SDLDebugRenderer2D final : public EDebugRenderer2D<SDLDebugRenderer2D>, public ESingleton<SDLDebugRenderer2D>
	{
	public:
		//--- Constructor & Destructor ---
		SDLDebugRenderer2D() = default;
		~SDLDebugRenderer2D() { Shutdown(); };

		//--- Functions ---
		void Initialize(Camera2D* const pActiveCamera);
		void Render();
		unsigned int LoadShadersToProgram(const char* const vertexShaderPath, const char* const fragmentShaderPath);
		unsigned int LoadShadersToProgramFromEmbeddedSource(const char* const vertexShader, const char* const fragmentShader);

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
		void DrawString(float worldPos_x, float worldPos_y, const char* const string, ...) const;
		void DrawString(const Elite::Vector2& worldPos, const char* const string, ...) const;
		void DrawString_ScreenSpace(const Elite::Vector2& screenPos, const char* const string, ...) const;

		//--- Util Functions ---
		inline float NextDepthSlice();

	private:
		//--- Datamembers ---
		//PROGRAM, VERTEX & ATTRIBUTE DATA
		unsigned int m_programID = 0;
		int m_projectionUniform = 0;
		int m_positionAttribute = 0;
		int m_colorAttribute = 1;
		int m_sizeAttribute = 2;
		unsigned int m_vaoId = 0;
		unsigned int m_bufferIDs[1] = {};

		//Functions
		void Shutdown();
		void DrawString_args(float screenPos_x, float screenPos_y, const char* const string, const va_list& args) const;
	};
}
#endif