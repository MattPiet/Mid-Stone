#pragma once
#include "Entities/Entity.h"

class Collision{
public:

	//Checks collision between two entities based on circle to circle collision
	static bool CheckCircleCircleCollision(const Entity& a, const Entity& b, const float radius1, const float radius2);
	
	//Checks collision between two entities based on quad to quad collision
	static bool CheckQuadQuadCollision(const Entity& entity_1, const Entity& entity_2);

	static void CollisionResponse(Entity& a, Entity& b);
};