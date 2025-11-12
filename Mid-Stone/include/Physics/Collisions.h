#pragma once
#include "Entities/Entity.h"
#include <Managers/2DActor.h>


class Collision{
public:

	//Checks collision between two entities based on circle to circle collision
	static bool CheckCircleCircleCollision(const Actor2D& entity_1, const Actor2D& entity_2);
	
	//Checks collision between two entities based on quad to quad collision
	static bool CheckQuadQuadCollision(const Actor2D& entity_1, const Actor2D& entity_2);

	static void CollisionResponse(Actor2D& a, Actor2D& b);


	static bool CheckOBBOBBCollision(const Actor2D& A, const Actor2D& B);
};