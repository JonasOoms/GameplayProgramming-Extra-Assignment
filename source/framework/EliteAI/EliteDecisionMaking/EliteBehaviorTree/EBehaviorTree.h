/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EBehaviorTree.h: Implementation of a BehaviorTree and the components of a Behavior Tree
/*=============================================================================*/
#ifndef ELITE_BEHAVIOR_TREE
#define ELITE_BEHAVIOR_TREE

//--- Includes ---
#include "framework/EliteAI/EliteData/EBlackboard.h"

namespace Elite
{
	//-----------------------------------------------------------------
	// BEHAVIOR TREE HELPERS
	//-----------------------------------------------------------------
	enum class BehaviorState
	{
		Failure,
		Success,
		Running
	};

	//-----------------------------------------------------------------
	// BEHAVIOR INTERFACES (BASE)
	//-----------------------------------------------------------------
	class IBehavior
	{
	public:
		IBehavior() = default;
		virtual ~IBehavior() = default;
		virtual BehaviorState Execute(Blackboard* const pBlackBoard) = 0;

	protected:
		BehaviorState m_CurrentState = BehaviorState::Failure;
	};

	//-----------------------------------------------------------------
	// BEHAVIOR TREE COMPOSITES (IBehavior)
	//-----------------------------------------------------------------
#pragma region COMPOSITES
	//--- COMPOSITE BASE ---
	class BehaviorComposite : public IBehavior
	{
	public:
		explicit BehaviorComposite(const std::vector<IBehavior*>& childBehaviors)
		{ m_ChildBehaviors = childBehaviors;	}
		virtual ~BehaviorComposite()
		{
			for (Elite::IBehavior* pb : m_ChildBehaviors)
				SAFE_DELETE(pb);
			m_ChildBehaviors.clear();
		}

		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override = 0;

	protected:
		std::vector<IBehavior*> m_ChildBehaviors = {};
	};

	//--- SELECTOR ---
	class BehaviorSelector : public BehaviorComposite
	{
	public:
		explicit BehaviorSelector(const std::vector<IBehavior*>& childBehaviors) :
			BehaviorComposite(childBehaviors) {}
		virtual ~BehaviorSelector() = default;

		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override;
	};

	//--- SEQUENCE ---
	class BehaviorSequence : public BehaviorComposite
	{
	public:
		explicit BehaviorSequence(const std::vector<IBehavior*>& childBehaviors) :
			BehaviorComposite(childBehaviors) {}
		virtual ~BehaviorSequence() = default;

		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override;
	};

	//--- PARTIAL SEQUENCE ---
	class BehaviorPartialSequence : public BehaviorSequence
	{
	public:
		explicit BehaviorPartialSequence(const std::vector<IBehavior*>& childBehaviors)
			: BehaviorSequence(childBehaviors) {}
		virtual ~BehaviorPartialSequence() = default;

		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override;

	private:
		unsigned int m_CurrentBehaviorIndex = 0;
	};
#pragma endregion

	//-----------------------------------------------------------------
	// BEHAVIOR TREE CONDITIONAL (IBehavior)
	//-----------------------------------------------------------------
	class BehaviorConditional : public IBehavior
	{
	public:
		explicit BehaviorConditional(std::function<bool(Blackboard*)> fp) : m_fpConditional(fp) {}
		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override;

	private:
		std::function<bool(Blackboard*)> m_fpConditional = nullptr;
	};

	//-----------------------------------------------------------------
	// BEHAVIOR TREE ACTION (IBehavior)
	//-----------------------------------------------------------------
	class BehaviorAction : public IBehavior
	{
	public:
		explicit BehaviorAction(std::function<BehaviorState(Blackboard*)> fp) : m_fpAction(fp) {}
		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override;

	private:
		std::function<BehaviorState(Blackboard*)> m_fpAction = nullptr;
	};

	//-----------------------------------------------------------------
	// BEHAVIOR TREE (BASE)
	//-----------------------------------------------------------------
	class BehaviorTree final : public Elite::IDecisionMaking
	{
	public:
		explicit BehaviorTree(Blackboard* const pBlackBoard, IBehavior* const pRootBehavior)
			: m_pBlackBoard(pBlackBoard), m_pRootBehavior(pRootBehavior) {};
		~BehaviorTree()
		{
			SAFE_DELETE(m_pRootBehavior);
			SAFE_DELETE(m_pBlackBoard); //Takes ownership of passed blackboard!
		};

		virtual void Update(float deltaTime) override
		{
			if (m_pRootBehavior == nullptr)
			{
				m_CurrentState = BehaviorState::Failure;
				return;
			}
				
			m_CurrentState = m_pRootBehavior->Execute(m_pBlackBoard);
		}
		Blackboard* GetBlackboard() const
		{ return m_pBlackBoard;	}

	private:
		BehaviorState m_CurrentState = BehaviorState::Failure;
		Blackboard* m_pBlackBoard = nullptr;
		IBehavior* m_pRootBehavior = nullptr;
	};

	//-----------------------------------------------------------------
	//DECORATORS
	//-----------------------------------------------------------------
	class BehaviorInverter : public IBehavior
	{
	public:
		explicit BehaviorInverter(IBehavior* const childBehavior) : m_pChildBehavior(childBehavior) {}
		virtual ~BehaviorInverter() { SAFE_DELETE(m_pChildBehavior); }
		virtual BehaviorState Execute(Blackboard* const pBlackBoard) override;
	private:
		IBehavior* m_pChildBehavior = nullptr;
	};
}
#endif