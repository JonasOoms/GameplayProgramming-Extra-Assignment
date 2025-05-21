#pragma once
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
class Flock;

//COHESION - FLOCKING
//*******************
class Cohesion : public Seek
{
public:
	Cohesion(Flock* const pFlock) :m_pFlock(pFlock) {};

	//Cohesion Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* const pAgent) override;

private:
	Flock* m_pFlock = nullptr;
};

class Separation : public Flee
{
public:
    Separation(Flock* const pFlock) :m_pFlock(pFlock) {};

    //Separation Behavior
    SteeringOutput CalculateSteering(float deltaT, SteeringAgent* const pAgent) override;

private:
    Flock* m_pFlock = nullptr;
};

//VELOCITY MATCH - FLOCKING
//************************
class VelocityMatch : public ISteeringBehavior
{
public:
    VelocityMatch(Flock* const pFlock) :m_pFlock(pFlock) {};

    //Velocity Match Behavior
    SteeringOutput CalculateSteering(float deltaT, SteeringAgent* const pAgent) override;

private:
    Flock* m_pFlock = nullptr;
};