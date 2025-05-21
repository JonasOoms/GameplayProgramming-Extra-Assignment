/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EWindowBase.h: base class representing a window (or screen) for Engine.
/*=============================================================================*/
#ifndef ELITE_WINDOW_BASE
#define	ELITE_WINDOW_BASE

//=== Includes & Forward Declarations ===
//struct Vector2;

namespace Elite
{
	//=== Window Parameters ===
	struct WindowParams final
	{
		std::string windowTitle = "AI Elite Framework";
		unsigned int width = 901;
		unsigned int height = 451;
		bool isResizable = false;
	};

	template<typename Impl>
	class EWindowBase
	{
	public:
		//=== Constructors & Destructors ===
		EWindowBase() {};
		~EWindowBase() {};

		//=== Window Functions ===
		void CreateEWindow(const WindowParams& params);
		void DestroyEWindow();
		void ProcedureEWindow();
		void ResizeEWindow(unsigned int width, unsigned int height);
		void RequestShutdown();

		bool ShutdownRequested() const { return m_ShutdownRequested; }
		const WindowParams& GetCurrentWindowParameters() const { return m_WindowParameters; }
		EliteRawWindow const GetRawWindowHandle() const;

	protected:
		//=== Datamembers ===
		WindowParams m_WindowParameters = {};
		bool m_ShutdownRequested = false;
	};
}
#endif