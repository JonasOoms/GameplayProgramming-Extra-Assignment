/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EPhysicsTypes.h: Common Physics Types
/*=============================================================================*/
#ifndef ELITE_PHYSICS_TYPES
#define ELITE_PHYSICS_TYPES

//namespace Elite
//{
	/*! EForce Mode */
	enum class EForceMode
	{
		eForce = 0,
		eImpulse = 1
	};
	/*! ERigidBody Type */
	enum class ERigidBodyType
	{
		eStatic = 0,
		eKinematic = 1,
		eDynamic = 2
	};
	/*!Base template for Transform information for Physics Implementations */
	template<typename PositionType, typename RotationType>
	struct ETransform final
	{
		PositionType position = {};
		RotationType rotation = {};

		ETransform(PositionType pos, RotationType rot)
			: position(pos), rotation(rot) {};
		ETransform() {};
	};
	/*!Structure that holds optional user defined data*/
	struct RigidBodyUserData final
	{
		int tag;
		void* pData;
	};
	/*!Initialization information for RigidBody creation*/
	struct RigidBodyDefine final
	{
		float linearDamping = 0.1f;
		float angularDamping = 0.01f;
		Elite::ERigidBodyType type = Elite::ERigidBodyType::eKinematic;
		bool allowSleep = false;

		RigidBodyDefine() {}
		RigidBodyDefine(float _linearDamping, float _angularDamping, Elite::ERigidBodyType _type, bool _allowSleep)
			:linearDamping(_linearDamping), angularDamping(_angularDamping), type(_type), allowSleep(_allowSleep)
		{}
	};
	/*!Raycast data helper*/
	template<typename rigidbodyType, typename positionType>
	struct ERaycastHitPoint final
	{
		std::vector<Elite::Vector2> vHitShapePoints = {}; //For FOV visualization purposes (should be unnecessary for other implementations)
		rigidbodyType* pRigidbody = nullptr;
		positionType Point = {};
		float Fraction = 0.0f;

		ERaycastHitPoint(rigidbodyType* _pRigidbody = nullptr, const positionType& _point = {}, float _fraction = 0.0f) :
			pRigidbody(_pRigidbody), Point(_point), Fraction(_fraction)
		{}
	};
	/*!Raycast Callback helper*/
	class ERaycastCallback
	{};
//}
#endif