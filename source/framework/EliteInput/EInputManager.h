/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EInputManager.h: manager class that controls the input in the engine.
/*=============================================================================*/
#ifndef ELITE_INPUT_MANAGER
#define	ELITE_INPUT_MANAGER

namespace Elite
{
	//=== Forward Declaration ===
#if (PLATFORM_ID == PLATFORM_WINDOWS)
	class SDLWindow;
#endif

	/*! EInputManager: manager class that controls all the input, captured from active platform & window*/
	class EInputManager final : public ESingleton<EInputManager>
	{
	public:
		bool IsKeyboardKeyDown(InputScancode key) const { return IsKeyPresent(InputType::eKeyboard, InputState::eDown, key); };
		bool IsKeyboardKeyUp(InputScancode key) const { return IsKeyPresent(InputType::eKeyboard, InputState::eReleased, key); }

		bool IsMouseButtonDown(InputMouseButton button) const { return IsMousePresent(InputType::eMouseButton, InputState::eDown, button); }
		bool IsMouseButtonUp(InputMouseButton button) const { return IsMousePresent(InputType::eMouseButton, InputState::eReleased, button); }
		bool IsMouseScrolling() const { return IsMousePresent(InputType::eMouseWheel); }
		bool IsMouseMoving() const { return IsMousePresent(InputType::eMouseMotion); }
		const MouseData& GetMouseData(InputType type, InputMouseButton button = InputMouseButton(0)) const;

	private:
		//=== Friends ===
		//Our window has access to add input events to our queue, our application can later use these events
#if (PLATFORM_ID == PLATFORM_WINDOWS)
		friend SDLWindow;
#endif

		//=== Internal Functions
		void Flush(){ m_InputContainer.clear();};
		void AddInputAction(const InputAction& inputAction) 
		{ m_InputContainer.push_back(inputAction); };

		bool IsKeyPresent(InputType type, InputState state, InputScancode code) const;
		bool IsMousePresent(InputType type, InputState state = InputState(0), InputMouseButton button = InputMouseButton(0)) const;

		//=== Datamembers ===
		std::vector<InputAction> m_InputContainer = {};
		MouseData m_InvalidMouseData = {};
	};
}
#endif