//=== General Includes ===
#include "stdafx.h"
#include "EInputManager.h"
using namespace Elite;

//=== Public Functions ===
const MouseData& EInputManager::GetMouseData(InputType type, InputMouseButton button) const
{
	auto result = std::find_if(m_InputContainer.begin(), m_InputContainer.end(),
		[type, button](const InputAction& ia)
	{
		return
			(ia.InputActionType == type) &&
			(ia.InputActionData.MouseInputData.Button == button);
	});

	if (result != m_InputContainer.end())
		return (*result).InputActionData.MouseInputData;
	return m_InvalidMouseData;
}

//=== Private Functions ===
bool EInputManager::IsKeyPresent(InputType type, InputState state, InputScancode code) const
{
	auto result = std::find_if(m_InputContainer.begin(), m_InputContainer.end(),
		[type, state, code](const InputAction& ia)
	{
		return
			(ia.InputActionType == type) &&
			(ia.InputActionState == state) &&
			(ia.InputActionData.KeyboardInputData.ScanCode == code);
	});
	return (result != m_InputContainer.end());
}

bool EInputManager::IsMousePresent(InputType type, InputState state, InputMouseButton button) const
{
	auto result = std::find_if(m_InputContainer.begin(), m_InputContainer.end(),
		[type, state, button](const InputAction& ia)
	{
		return
			(ia.InputActionType == type) &&
			(ia.InputActionState == state) &&
			(ia.InputActionData.MouseInputData.Button == button);
	});
	return (result != m_InputContainer.end());
}