//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "SDLDebugRenderer2D.h"
using namespace Elite;

//Immediate Draw Auto Layering Settings
#define DEPTH_SLICE_OFFSET 0.0005f
#define DEPTH_SLICE_FINE_OFFSET 0.0000005f
#define DEPTH_SLICE_MIN -0.5f//far
#define DEPTH_SLICE_MAX 0.5f//close

//Functions
void SDLDebugRenderer2D::Initialize(Camera2D* const pActiveCamera)
{
	//Store variables
	m_pActiveCamera = pActiveCamera;

	//Initialize container sizes - reserve continious memory
	const int initialSize = 512;
	m_vPoints.reserve(initialSize);
	m_vLines.reserve(initialSize);
	m_vTriangles.reserve(initialSize);

	//Create the programs we use in our framework
	m_programID = DEBUGRENDERER2D->LoadShadersToProgramFromEmbeddedSource(DefaultVertexShaderSource, DefaultFragmentShaderSource);
	//Get uniform shader attributes
	m_projectionUniform = glGetUniformLocation(m_programID, "projectionMatrix");

	//Generate buffers and Link attributes
	glGenVertexArrays(1, &m_vaoId);
	glGenBuffers(1, m_bufferIDs);
	glBindVertexArray(m_vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferIDs[0]);

	//Specify the INTERLEAVED layout in vertices vector (MIND the SIZE and the STRIDE)!
	glVertexAttribPointer(m_positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glEnableVertexAttribArray(m_positionAttribute);
	glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
	glEnableVertexAttribArray(m_colorAttribute);
	glVertexAttribPointer(m_sizeAttribute, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, size)));
	glEnableVertexAttribArray(m_sizeAttribute);

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Support Depth
	glEnable(GL_DEPTH_TEST);

	//Check for errors
	const GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR)
	{
		fprintf(stderr, "OpenGL error = %d\n", errCode);
		assert(false);
	}
}

void SDLDebugRenderer2D::Render()
{
	//Clear color
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Set program to use for rendering
	glUseProgram(m_programID);

	//Bind Buffers
	glBindVertexArray(m_vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferIDs[0]);

	//Build projection matrix and push to program
	float proj[16] = { 0.0f };
	m_pActiveCamera->BuildProjectionMatrix(proj, 0.0f);
	glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);

	//Copy Data and Draw Lines
	int size = static_cast<int>(m_vLines.size());
	if (size > 0)
	{
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), &m_vLines.front(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINES, 0, size);
	}

	//Copy Data and Draw Triangles
	size = static_cast<int>(m_vTriangles.size());
	if (size > 0)
	{
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), &m_vTriangles.front(), GL_DYNAMIC_DRAW);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, size);
		glDisable(GL_BLEND);
	}
	
	//Copy Data and Draw Point
	size = static_cast<int>(m_vPoints.size());
	if (size > 0)
	{
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), &m_vPoints.front(), GL_DYNAMIC_DRAW);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, size);
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	//Cleanup containers
	m_vTriangles.clear();
	m_vLines.clear();
	m_vPoints.clear();
	//Cleanup OpenGL
	glDisable(GL_PROGRAM_POINT_SIZE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glFlush();

	//Reset DepthSlice
	m_CurrDepthSlice = DEPTH_SLICE_MAX;

	//Search for errors
	const GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR)
	{
		fprintf(stderr, "OpenGL error = %d\n", errCode);
		assert(false);
	}
}

void SDLDebugRenderer2D::Shutdown()
{
	m_vPoints.clear();
	m_vLines.clear();
	m_vTriangles.clear();

	glDeleteBuffers(1, m_bufferIDs);
	glDeleteVertexArrays(1, &m_vaoId);
	glDeleteProgram(m_programID);
}

void SDLDebugRenderer2D::DrawPolygon(Elite::Polygon* polygon, const Elite::Color& color, float depth)
{
	//Copy data to vector
	std::vector<Elite::Vector2> points;
	points.assign(polygon->GetPoints().begin(), polygon->GetPoints().end());

	//Draw copied list as lines
	depth -= DEPTH_SLICE_FINE_OFFSET;
	Vector2 p1 = points[points.size() - 1];
	int count = static_cast<int>(points.size());
	for (int i = 0; i < count; ++i)
	{
		const Elite::Vector2& p2 = points[i];
		m_vLines.push_back(Vertex(p1, depth, color));
		m_vLines.push_back(Vertex(p2, depth, color));
		p1 = p2;
	}

	//Also draw children
	for (const Elite::Polygon& child : polygon->GetChildren())
	{
		points.clear();
		points.assign(child.GetPoints().begin(), child.GetPoints().end());

		p1 = points[points.size() - 1];
		count = static_cast<int>(points.size());
		for (int i = 0; i < count; ++i)
		{
			const Elite::Vector2& p2 = points[i];
			m_vLines.push_back(Vertex(p1, depth, color));
			m_vLines.push_back(Vertex(p2, depth, color));
			p1 = p2;
		}
	}
}

void SDLDebugRenderer2D::DrawPolygon(Elite::Polygon* polygon, const Elite::Color& color)
{
	DrawPolygon(polygon, color, NextDepthSlice());
}

void SDLDebugRenderer2D::DrawPolygon(const Elite::Vector2* points, int count, const Elite::Color& color, float depth)
{
	Elite::Vector2 p1 = points[count - 1];
	for (int i = 0; i < count; ++i)
	{
		const Elite::Vector2& p2 = points[i];
		m_vLines.push_back(Vertex(p1, depth, color));
		m_vLines.push_back(Vertex(p2, depth, color));
		p1 = p2;
	}
}

void SDLDebugRenderer2D::DrawSolidPolygon(Elite::Polygon* const polygon, const Elite::Color& color, float depth, bool triangulate)
{
	//Color
	const Elite::Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

	//Copy data to vector
	std::vector<Elite::Vector2> points;
	points.assign(polygon->GetPoints().begin(), polygon->GetPoints().end());

	//Triangulation
	std::vector<Elite::Triangle*> triangles;
	if (triangulate || (!triangulate && !polygon->IsTriangulated())) //Triangulate if requested OR when necessary!
		triangles = polygon->Triangulate();
	else
		triangles = polygon->GetTriangles();

	//Duplicate code because of possible triangulation with children -> cannot call DrawSolidPolygon directly (like Box2D)
	//Else we would have "double triangulation"!! 
	int count = static_cast<int>(triangles.size());
	for (int i = 0; i < count; ++i)
	{
		m_vTriangles.push_back(Vertex(triangles[i]->p1, depth, fillColor));
		m_vTriangles.push_back(Vertex(triangles[i]->p2, depth, fillColor));
		m_vTriangles.push_back(Vertex(triangles[i]->p3, depth, fillColor));

		//TEST
		Elite::Vector2 pos = Elite::Vector2(triangles[i]->p1 + triangles[i]->p2 + triangles[i]->p3) / 3.0f;
		DrawPoint(pos, 2, Elite::Color(0, 1, 0, 1));
	}

	//Draw copied list as lines
	const bool drawLines = false;
	depth -= DEPTH_SLICE_FINE_OFFSET;
	if (drawLines)
	{
		Elite::Vector2 p1 = points[points.size() - 1];
		count = static_cast<int>(points.size());
		for (int i = 0; i < count; ++i)
		{
			Elite::Vector2& p2 = points[i];
			m_vLines.push_back(Vertex(p1, depth, color));
			m_vLines.push_back(Vertex(p2, depth, color));
			p1 = p2;
		}
	}
	//Draw Wireframe
	const bool drawWireframe = true;
	const Elite::Color wireFrameColor(0, 0.5f * 1, 0, 0.5f);
	if (drawWireframe)
	{
		count = static_cast<int>(triangles.size());
		for (int i = 0; i < count; ++i)
		{
			m_vLines.push_back(Vertex(triangles[i]->p1, depth, wireFrameColor));
			m_vLines.push_back(Vertex(triangles[i]->p2, depth, wireFrameColor));
			m_vLines.push_back(Vertex(triangles[i]->p2, depth, wireFrameColor));
			m_vLines.push_back(Vertex(triangles[i]->p3, depth, wireFrameColor));
			m_vLines.push_back(Vertex(triangles[i]->p3, depth, wireFrameColor));
			m_vLines.push_back(Vertex(triangles[i]->p1, depth, wireFrameColor));
		}
	}
}

void SDLDebugRenderer2D::DrawSolidPolygon(const Elite::Polygon* polygon, const Elite::Color& color, float depth)
{
	//We will const cast, because we will not change the polygon for sure as triangulation is false! Not the best way though...
	Elite::Polygon* p = const_cast<Elite::Polygon*>(polygon);
	DrawSolidPolygon(p, color, depth, false);
}

void SDLDebugRenderer2D::DrawSolidPolygon(const Elite::Vector2* points, int count, const Elite::Color& color, float depth, bool triangluate)
{
	//Color
	const Elite::Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

	if (triangluate)
	{
		std::vector<Elite::Triangle*> verts = {};
		Elite::Polygon polygon(points, count);
		verts = polygon.Triangulate();

		//Draw Triangles
		const int size = static_cast<int>(verts.size());
		for (int i = 0; i < size; ++i)
		{
			m_vTriangles.push_back(Vertex(verts[i]->p1, depth, fillColor));
			m_vTriangles.push_back(Vertex(verts[i]->p2, depth, fillColor));
			m_vTriangles.push_back(Vertex(verts[i]->p3, depth, fillColor));
		}
	}
	else
	{
		std::vector<Elite::Vector2> verts = {};
		for (int i = 0; i < count; ++i)
			verts.push_back(points[i]);

		//Draw Triangles
		const int size = static_cast<int>(verts.size() - 1);
		for (int i = 1; i < size; ++i)
		{
			m_vTriangles.push_back(Vertex(verts[0], depth, fillColor));
			m_vTriangles.push_back(Vertex(verts[i], depth, fillColor));
			m_vTriangles.push_back(Vertex(verts[i + 1], depth, fillColor));
		}
	}

	//Draw Lines
	depth -= DEPTH_SLICE_FINE_OFFSET;
	Elite::Vector2 p1 = points[count - 1];
	for (int i = 0; i < count; ++i)
	{
		const Elite::Vector2& p2 = points[i];
		m_vLines.push_back(Vertex(p1, depth, color));
		m_vLines.push_back(Vertex(p2, depth, color));
		p1 = p2;
	}
}

void SDLDebugRenderer2D::DrawCircle(const Elite::Vector2& center, float radius, const Elite::Color& color, float depth)
{
	const int k_segments = 16;
	const float k_increment = 2.0f * b2_pi / k_segments;
	const float sinInc = sinf(k_increment);
	const float cosInc = cosf(k_increment);
	Elite::Vector2 r1(1.0f, 0.0f);
	Elite::Vector2 v1 = center + radius * r1;
	for (int i = 0; i < k_segments; ++i)
	{
		// Perform rotation to avoid additional trigonometry.
		Elite::Vector2 r2;
		r2.x = cosInc * r1.x - sinInc * r1.y;
		r2.y = sinInc * r1.x + cosInc * r1.y;
		const Elite::Vector2 v2 = center + radius * r2;
		m_vLines.push_back(Vertex(v1, depth, color));
		m_vLines.push_back(Vertex(v2, depth, color));
		r1 = r2;
		v1 = v2;
	}
}

void SDLDebugRenderer2D::DrawSolidCircle(const Elite::Vector2& center, float radius, const Elite::Vector2& axis, const Elite::Color& color, float depth)
{
	const int k_segments = 16;
	const float k_increment = 2.0f * b2_pi / k_segments;
	const float sinInc = sinf(k_increment);
	const float cosInc = cosf(k_increment);
	const Elite::Vector2& v0 = center;
	Elite::Vector2 r1(cosInc, sinInc);
	Elite::Vector2 v1 = center + radius * r1;
	const Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, color.a);
	for (int i = 0; i < k_segments; ++i)
	{
		// Perform rotation to avoid additional trigonometry.
		Elite::Vector2 r2;
		r2.x = cosInc * r1.x - sinInc * r1.y;
		r2.y = sinInc * r1.x + cosInc * r1.y;
		const Elite::Vector2 v2 = center + radius * r2;
		m_vTriangles.push_back(Vertex(v0, depth, fillColor));
		m_vTriangles.push_back(Vertex(v1, depth, fillColor));
		m_vTriangles.push_back(Vertex(v2, depth, fillColor));
		r1 = r2;
		v1 = v2;
	}

	r1 = Elite::Vector2(1.0f, 0.0f);
	v1 = center + radius * r1;
	depth -= DEPTH_SLICE_FINE_OFFSET;

	for (int i = 0; i < k_segments; ++i)
	{
		Elite::Vector2 r2;
		r2.x = cosInc * r1.x - sinInc * r1.y;
		r2.y = sinInc * r1.x + cosInc * r1.y;
		const Elite::Vector2 v2 = center + radius * r2;
		m_vLines.push_back(Vertex(v1, depth, color));
		m_vLines.push_back(Vertex(v2, depth, color));
		r1 = r2;
		v1 = v2;
	}

	// Draw a line fixed in the circle to animate rotation.
	const Elite::Vector2 p = center + radius * axis;
	depth -= DEPTH_SLICE_FINE_OFFSET;

	m_vLines.push_back(Vertex(center, depth, color));
	m_vLines.push_back(Vertex(p, depth, color));
}

void SDLDebugRenderer2D::DrawSolidCircle(const Elite::Vector2& center, float radius, const Elite::Vector2& axis, const Elite::Color& color)
{
	DrawSolidCircle(center, radius, axis, color, NextDepthSlice());
}

void SDLDebugRenderer2D::DrawSegment(const Elite::Vector2& p1, const Elite::Vector2& p2, const Elite::Color& color, float depth)
{
	m_vLines.push_back(Vertex(p1, depth, color));
	m_vLines.push_back(Vertex(p2, depth, color));
}

void SDLDebugRenderer2D::DrawSegment(const Elite::Vector2& p1, const Elite::Vector2& p2, const Elite::Color& color)
{
	DrawSegment(p1, p2, color, NextDepthSlice());
}

void SDLDebugRenderer2D::DrawDirection(const Elite::Vector2& p, const Elite::Vector2& dir, float length, const Color& color, float depth)
{
	DrawSegment(p, p + (dir.GetNormalized()*length), color, depth);
}

void SDLDebugRenderer2D::DrawTransform(const Elite::Vector2& p, const Elite::Vector2& xAxis, const Elite::Vector2& yAxis, float depth)
{
	const float k_axisScale = 0.4f;
	const Elite::Color red(1.0f, 0.0f, 0.0f);
	const Elite::Color green(0.0f, 1.0f, 0.0f);
	Elite::Vector2 p2 = {};

	m_vLines.push_back(Vertex(p, depth, red));
	p2 = p + k_axisScale * xAxis;
	m_vLines.push_back(Vertex(p2, depth, red));

	m_vLines.push_back(Vertex(p, depth, green));
	p2 = p + k_axisScale * yAxis;
	m_vLines.push_back(Vertex(p2, depth, green));
}


void SDLDebugRenderer2D::DrawPoint(const Elite::Vector2& p, float size, const Elite::Color& color, float depth)
{
	m_vPoints.push_back(Vertex(p, depth, color, size));
}

void SDLDebugRenderer2D::DrawString(float worldPos_x, float worldPos_y, const char* string, ...) const
{
	const Elite::Vector2 screenPos = m_pActiveCamera->ConvertWorldToScreen({ worldPos_x , worldPos_y });

	va_list arg;
	va_start(arg, string);

	DrawString_args(screenPos.x, screenPos.y, string, arg);

	va_end(arg);
}

void SDLDebugRenderer2D::DrawString(const Elite::Vector2& worldPos, const char* string, ...) const
{
	if (!m_pActiveCamera)
		return;

	const Elite::Vector2 screenPos = m_pActiveCamera->ConvertWorldToScreen(worldPos);

	va_list arg;
	va_start(arg, string);

	DrawString_args(screenPos.x, screenPos.y, string, arg);

	va_end(arg);
}

void SDLDebugRenderer2D::DrawString_ScreenSpace(const Elite::Vector2& screenPos, const char* string, ...) const
{
	va_list arg;
	va_start(arg, string);

	DrawString_args(screenPos.x, screenPos.y, string, arg);

	va_end(arg);
}

void SDLDebugRenderer2D::DrawString_args(float screenPos_x, float screenPos_y, const char* string, const va_list& args) const
{
	//TODO: ADD CLEAN TEXRENDERING
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 colorWindowBg = style.Colors[ImGuiCol_WindowBg];
	const float initialAlpha = colorWindowBg.w;
	colorWindowBg.w = 0.0f;
	style.Colors[ImGuiCol_WindowBg] = colorWindowBg;

	ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetCursorPos(ImVec2(screenPos_x,screenPos_y));
	ImGui::TextColoredV(ImColor(230, 153, 153, 255), string, args);
	ImGui::End();

	//Reset alpha
	colorWindowBg.w = initialAlpha;
	style.Colors[ImGuiCol_WindowBg] = colorWindowBg;
}

inline float SDLDebugRenderer2D::NextDepthSlice()
{
	m_CurrDepthSlice -= DEPTH_SLICE_OFFSET;

	if (m_CurrDepthSlice < DEPTH_SLICE_MIN)
		m_CurrDepthSlice = DEPTH_SLICE_MAX;

	return m_CurrDepthSlice;
}

unsigned int SDLDebugRenderer2D::LoadShadersToProgram(const char* const vertexShaderPath, const char* const fragmentShaderPath)
{
	//Create shaders
	const GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//Read the Vertex Shader code from external file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexShaderPath, std::ios::in);
	if (vertexShaderStream.is_open())
	{
		std::string line = "";
		while (std::getline(vertexShaderStream, line))
			vertexShaderCode += "\n" + line;
		vertexShaderStream.close();
	}
	else
	{
		printf("Impossible to open vertex shader file: %s!", vertexShaderPath);
		const int rv = getchar();
		return 0;
	}

	//Read the Fragment Shader code from external file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentShaderPath, std::ios::in);
	if (fragmentShaderStream.is_open())
	{
		std::string line = "";
		while (std::getline(fragmentShaderStream, line))
			fragmentShaderCode += "\n" + line;
		fragmentShaderStream.close();
	}
	else
	{
		printf("Impossible to open fragment shader file: %s!", fragmentShaderPath);
		const int rv = getchar();
		return 0;
	}

	//Compilation info
	int result = GL_FALSE;
	int infoLogLength = 0;
	//Compile Vertex Shader
	const char* vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, nullptr);
	glCompileShader(vertexShaderID);
	//Check compilation
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
		printf("%s \n", &shaderErrorMessage[0]);
	}

	//Compile Fragment Shader
	const char* fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, nullptr);
	glCompileShader(fragmentShaderID);
	//Check compilation
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
		printf("%s \n", &shaderErrorMessage[0]);
	}

	//Link the program
	const GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//glBindFragDataLocation(programID, 0, "color");
	glLinkProgram(programID);

	//Check program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> programErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, nullptr, &programErrorMessage[0]);
		printf("%s \n", &programErrorMessage[0]);
	}

	//Detach and Delete shader objects before exiting, which will flag object, so as soon as
	//our program gets deleted everything cleans up nicely
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//IMPORTANT, return our program ID!
	return programID;
}

unsigned int SDLDebugRenderer2D::LoadShadersToProgramFromEmbeddedSource(const char* const vertexShader, const char* const fragmentShader)
{
	//Create shaders
	const GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//Compilation info
	int result = GL_FALSE;
	int infoLogLength = 0;
	//Compile Vertex Shader
	glShaderSource(vertexShaderID, 1, &vertexShader, nullptr);
	glCompileShader(vertexShaderID);
	//Check compilation
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
		printf("%s \n", &shaderErrorMessage[0]);
	}

	//Compile Fragment Shader
	glShaderSource(fragmentShaderID, 1, &fragmentShader, nullptr);
	glCompileShader(fragmentShaderID);
	//Check compilation
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
		printf("%s \n", &shaderErrorMessage[0]);
	}

	//Link the program
	const GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	//Check program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> programErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, nullptr, &programErrorMessage[0]);
		printf("%s \n", &programErrorMessage[0]);
	}

	//Detach and Delete shader objects before exiting, which will flag object, so as soon as
	//our program gets deleted everything cleans up nicely
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//IMPORTANT, return our program ID!
	return programID;
}