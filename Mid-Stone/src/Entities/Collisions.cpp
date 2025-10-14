#include "Entities/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Entity& ent1, const Entity& ent2){
	//Distance between the centers of the circles
	float distance = MATH::VMath::distance(ent1.GetPosition(), ent2.GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float radiusSum = (ent1.GetScale().x / 2.0f) + (ent2.GetScale().x / 2.0f); //Assuming scale.x is the same as scale.y 
	return distance <= radiusSum;
}

bool Collision::CheckQuadQuadCollision(const Entity& ent1, const Entity& ent2) {
	//Get the min and max points of the first quad
	MATH::Vec3 ent1Min = ent1.GetPosition() - (ent1.GetScale() / 2.0f);
	MATH::Vec3 ent1Max = ent1.GetPosition() + (ent1.GetScale() / 2.0f);
	//Get the min and max points of the second quad
	MATH::Vec3 ent2Min = ent2.GetPosition() - (ent2.GetScale() / 2.0f);
	MATH::Vec3 ent2Max = ent2.GetPosition() + (ent2.GetScale() / 2.0f);
	//Check for overlap on all axes
	bool overlapX = (ent1Min.x <= ent2Max.x) && (ent1Max.x >= ent2Min.x);
	bool overlapY = (ent1Min.y <= ent2Max.y) && (ent1Max.y >= ent2Min.y);
	bool overlapZ = (ent1Min.z <= ent2Max.z) && (ent1Max.z >= ent2Min.z);
	return overlapX && overlapY && overlapZ;
}