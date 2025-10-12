#pragma once
#include "Entities/Entity.h"

class Collision{
public:
	//Checks collision between two entities based on circle to circle collision
	bool CheckCircleCircleCollision(const Entity& a, const Entity& b);
	
	//Checks collision between two entities based on quad to quad collision
	bool CheckQuadQuadCollision(const Entity& a, const Entity& b);
};