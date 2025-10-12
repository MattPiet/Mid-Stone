#include "Entities/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Entity& ent1, const Entity& ent2){
	//Distance between the centers of the circles
	float distance = MATH::VMath::distance(ent1.GetPosition(), ent2.GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float radiusSum = (ent1.GetScale().x / 2.0f) + (ent2.GetScale().x / 2.0f); //Assuming scale.x is the same as scale.y 
	return distance <= radiusSum;
}