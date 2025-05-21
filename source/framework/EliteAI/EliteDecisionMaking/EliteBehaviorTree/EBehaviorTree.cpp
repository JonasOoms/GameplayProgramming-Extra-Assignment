//=== General Includes ===
#include "stdafx.h"
#include "EBehaviorTree.h"
using namespace Elite;

//-----------------------------------------------------------------
// BEHAVIOR TREE COMPOSITES (IBehavior)
//-----------------------------------------------------------------
#pragma region COMPOSITES

//SELECTOR
BehaviorState BehaviorSelector::Execute(Blackboard* const pBlackBoard)
{
	// BT TODO:
	//TODO: Fill in this code
	// Loop over all children in m_ChildBehaviors

		//Every Child: Execute and store the result in m_CurrentState

		//Check the currentstate and apply the selector Logic:
		//if a child returns Success:
			//stop looping over all children and return Success

		//if a child returns Running:
			//Running: stop looping and return Running


		//The selector fails if all children failed.

	//All children failed
	m_CurrentState = BehaviorState::Failure;
	return m_CurrentState;
}

//SEQUENCE
BehaviorState BehaviorSequence::Execute(Blackboard* const pBlackBoard)
{
	// BT TODO:
	//TODO: FIll in this code
	//Loop over all children in m_ChildBehaviors

		//Every Child: Execute and store the result in m_CurrentState

		//Check the currentstate and apply the sequence Logic:
		//if a child returns Failed:
			//stop looping over all children and return Failed

		//if a child returns Running:
			//Running: stop looping and return Running

		//The selector succeeds if all children succeeded.

	//All children succeeded 
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;
}

//PARTIAL SEQUENCE
BehaviorState BehaviorPartialSequence::Execute(Blackboard* const pBlackBoard)
{
	while (m_CurrentBehaviorIndex < static_cast<int>(m_ChildBehaviors.size()))
	{
		m_CurrentState = m_ChildBehaviors[m_CurrentBehaviorIndex]->Execute(pBlackBoard);
		switch (m_CurrentState)
		{
		case BehaviorState::Failure:
			m_CurrentBehaviorIndex = 0;
			return m_CurrentState;
		case BehaviorState::Success:
			++m_CurrentBehaviorIndex;
			m_CurrentState = BehaviorState::Running;
			return m_CurrentState;
		case BehaviorState::Running:
			return m_CurrentState;
		}
	}

	m_CurrentBehaviorIndex = 0;
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;
}
#pragma endregion

//-----------------------------------------------------------------
// BEHAVIOR TREE CONDITIONAL (IBehavior)
//-----------------------------------------------------------------
BehaviorState BehaviorConditional::Execute(Blackboard* const pBlackBoard)
{
	if (m_fpConditional == nullptr)
		return BehaviorState::Failure;

	// This used to be a switch case for some reason, now it's not, be happy :)
	if (m_fpConditional(pBlackBoard))
	{
		m_CurrentState = BehaviorState::Success;
	}
	else
	{
		m_CurrentState = BehaviorState::Failure;
	}
	return m_CurrentState;
}

//-----------------------------------------------------------------
// BEHAVIOR TREE ACTION (IBehavior)
//-----------------------------------------------------------------
BehaviorState BehaviorAction::Execute(Blackboard* const pBlackBoard)
{
	if (m_fpAction == nullptr)
		return BehaviorState::Failure;

	m_CurrentState = m_fpAction(pBlackBoard);
	return m_CurrentState;
}

//-----------------------------------------------------------------
//DECORATORS
//-----------------------------------------------------------------
BehaviorState Elite::BehaviorInverter::Execute(Blackboard* const pBlackBoard)
{
	BehaviorState state = m_pChildBehavior->Execute(pBlackBoard);
	switch (state)
	{
	case Elite::BehaviorState::Failure:
		return BehaviorState::Success;

	case Elite::BehaviorState::Success:
		return BehaviorState::Failure;
	default:
		return state;
	}
}