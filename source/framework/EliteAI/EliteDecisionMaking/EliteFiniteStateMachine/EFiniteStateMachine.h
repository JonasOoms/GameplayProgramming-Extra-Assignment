/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
// Authors: Andries Geens
// http://www.gameaipro.com/GameAIPro3/GameAIPro3_Chapter12_A_Reusable_Light-Weight_Finite-State_Machine.pdf
/*=============================================================================*/
// EStateMachine.h: Other implementation of a FSM
/*=============================================================================*/
#ifndef ELITE_FINITE_STATE_MACHINE
#define ELITE_FINITE_STATE_MACHINE

namespace Elite
{
	class FSMState
	{
	public:
		FSMState(){}
		virtual ~FSMState() = default;

		virtual void OnEnter(Blackboard* const pBlackboard) {};
		virtual void OnExit(Blackboard* const pBlackboard) {};
		virtual void Update(Blackboard* const pBlackboard, float deltaTime) {};
	};

	class FSMCondition
	{
	public:
		FSMCondition() = default;
		virtual ~FSMCondition() = default;
		virtual bool Evaluate(Blackboard* const pBlackboard) const = 0;
	};

	class FiniteStateMachine final : public Elite::IDecisionMaking
	{
	public:
		FiniteStateMachine(FSMState* const startState, Blackboard* const pBlackboard);
		virtual ~FiniteStateMachine();
		
		void AddTransition(FSMState* const startState, FSMState* const toState, FSMCondition* const transition);
		virtual void Update(float deltaTime) override;
		Elite::Blackboard* const GetBlackboard() const;

	private:
		void ChangeState(FSMState* const newState);

	private:
		typedef std::pair<FSMCondition*, FSMState*> TransitionStatePair;
		typedef std::vector<TransitionStatePair> Transitions;

		std::map<FSMState*, Transitions> m_Transitions; //Key is the state, value are all the transitions for that current state 
		FSMState* m_pCurrentState;
		Blackboard* m_pBlackboard = nullptr; // takes ownership of the blackboard
	};
}
#endif