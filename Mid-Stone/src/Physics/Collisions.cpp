#include "Physics/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Entity& entity_1, const Entity& entity_2){
	//Check for overlap on all axes
		//Distance between the centers of the circles
	float distance = MATH::VMath::distance(entity_1.GetPosition(), entity_2.GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float widthSum = entity_1.GetHitbox().x + entity_2.GetHitbox().x;
	float heightSum = entity_1.GetHitbox().y + entity_2.GetHitbox().y;
	return distance <= widthSum && distance <= heightSum;
}

bool Collision::CheckQuadQuadCollision(const Entity& entity_1, const Entity& entity_2) {
	//Testing Quad to Quad collision
	float minx1 = entity_1.GetPosition().x - entity_1.GetHitbox().x / 2.0f;
	float maxx1 = entity_1.GetPosition().x + entity_1.GetHitbox().x / 2.0f;
	float miny1 = entity_1.GetPosition().y - entity_1.GetHitbox().y / 2.0f;
	float maxy1 = entity_1.GetPosition().y + entity_1.GetHitbox().y / 2.0f;
	
	float minx2 = entity_2.GetPosition().x - entity_2.GetHitbox().x / 2.0f;
	float maxx2 = entity_2.GetPosition().x + entity_2.GetHitbox().x / 2.0f;
	float miny2 = entity_2.GetPosition().y - entity_2.GetHitbox().y / 2.0f;
	float maxy2 = entity_2.GetPosition().y + entity_2.GetHitbox().y / 2.0f;

	bool overlapx = (minx1 <= maxx2) && (maxx1 >= minx2);
	bool overlapy = (miny1 <= maxy2) && (maxy1 >= miny2);

	return overlapx && overlapy;
}

void Collision::CollisionResponse(Entity& a, Entity& b) {
	if(CheckQuadQuadCollision(a,b) && !a.IsExpired() && !b.IsExpired()){
		a.SetHitboxColor(MATH::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
		b.SetHitboxColor(MATH::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
	}
	else{
		a.SetHitboxColor(MATH::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
		b.SetHitboxColor(MATH::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
	}
}
