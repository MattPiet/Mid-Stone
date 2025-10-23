#include "Physics/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Entity& ent1, const Entity& ent2, const float radius1, const float radius2){
	//Distance between the centers of the circles
	float distance = MATH::VMath::distance(ent1.GetPosition(), ent2.GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float radiusSum = radius1 + radius2;
	return distance <= radiusSum;
}

bool Collision::CheckQuadQuadCollision(const Entity& entity_1, const Entity& entity_2) {
	//Check for overlap on all axes
		//Distance between the centers of the circles
	float distance = MATH::VMath::distance(entity_1.GetPosition(), entity_2.GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float widthSum = entity_1.GetHitbox().x/2.0f + entity_2.GetHitbox().x/2.0f;
	float heightSum = entity_1.GetHitbox().y/2.0f + entity_2.GetHitbox().y/2.0f;
	return distance <= widthSum && distance <= heightSum;
}

void Collision::CollisionResponse(Entity& a, Entity& b) {
	a.SetHitboxColor(MATH::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
	b.SetHitboxColor(MATH::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
}
