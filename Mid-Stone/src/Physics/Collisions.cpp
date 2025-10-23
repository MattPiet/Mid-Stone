#include "Physics/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Entity& ent1, const Entity& ent2, const float radius1, const float radius2){
	//Distance between the centers of the circles
	float distance = MATH::VMath::distance(ent1.GetPosition(), ent2.GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float radiusSum = radius1 + radius2;
	return distance <= radiusSum;
}

bool Collision::CheckQuadQuadCollision(const Entity& ent1, const Entity& ent2, const MATH::Vec3 ent1Min, const MATH::Vec3 ent1Max, const MATH::Vec3 ent2Min, const MATH::Vec3 ent2Max) {
	//Check for overlap on all axes
	bool overlapX = (ent1Min.x <= ent2Max.x) && (ent1Max.x >= ent2Min.x);
	bool overlapY = (ent1Min.y <= ent2Max.y) && (ent1Max.y >= ent2Min.y);
	
	return overlapX && overlapY;
}

//void Collision::CollisionResponse(Entity& a, Entity& b) {
//	if ()
//}
