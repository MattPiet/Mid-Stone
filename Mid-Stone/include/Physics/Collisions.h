#pragma once
#include "Entities/Entity.h"

class Collision{
public:
	//Checks collision between two entities based on circle to circle collision
	bool CheckCircleCircleCollision(const Entity& a, const Entity& b, const float radius1, const float radius2);
	
	//Checks collision between two entities based on quad to quad collision
	bool CheckQuadQuadCollision(const Entity& a, const Entity& b, const MATH::Vec3 ent1Min, const MATH::Vec3 ent1Max, const MATH::Vec3 ent2Min, const MATH::Vec3 ent2Max);

	void CollisionResponse(Entity& a, Entity& b);
};