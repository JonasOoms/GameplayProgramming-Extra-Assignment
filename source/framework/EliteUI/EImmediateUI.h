/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EImmediateUI.h: ImGUI implementation for Immediate UI
/*=============================================================================*/
#ifndef ELITE_IMMEDIATEUI
#define ELITE_IMMEDIATEUI

struct ImDrawData;

namespace Elite
{
#ifdef PLATFORM_WINDOWS
	class EImmediateUI final
	{
	public:
		//--- Constructor & Destructor ---
		EImmediateUI() = default;
		~EImmediateUI();

		//--- UI Functions ---
		void Initialize(EliteRawWindow const pWindow);
		void Render();
		void EventProcessing();
		static void StaticRender(ImDrawData* const drawData);
		void NewFrame(EliteRawWindow const pWindow, float deltaTime);
		bool FocusedOnUI();

	private:
		//--- Datamembers ---
		static float m_sMouseWheel;
		static bool m_sMousePressed[3];
		unsigned int m_atlasTextureID = 0;

		static GLuint m_programID;
		static GLuint m_vboID, m_vaoID, m_elementsID;
		static GLint m_textureUniform, m_projectionUniform;
		static GLint m_positionAttribute, m_uvAttribute, m_colorAttribute;

		//Functions
		static void SetClipboardText(const char* const text);
		static const char* GetClipboardText();
		void SetupStyle();

		//C++ make the class non-copyable
		EImmediateUI(const EImmediateUI&) = default;
		EImmediateUI& operator=(const EImmediateUI&) = default;
	};
#elif
	class EImmediateUI final
	{
	public:
		//--- UI Functions ---
		void Initialize(EliteRawWindow const pWindow){};
		void Render(){};
		void EventProcessing(){};
		static void StaticRender(ImDrawData* const drawData){};
		void NewFrame(EliteRawWindow const pWindow, float deltaTime){};
		bool FocusedOnUI() { return false; }
	};
#endif
}
#endif