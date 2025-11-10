#include "Physics/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Entity& entity_1, const Entity& entity_2){
	// TODO This is wrong, rework later.
	
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

struct OBB2D {
	Vec2 center;     // world position
	Vec2 halfExtents;   // half-width (x), half-height (y)
	float orientation;  // radians (counterclockwise)
	// cached local axes (unit vectors)
	Vec2 xAxis;
	Vec2 yAxis;
};
// Extract yaw (rotation about Z) from quaternion (in radians).
// Normalizes the quaternion before extraction.
static float Get2DRotationFromQuaternion(const MATH::Quaternion& q_in) {
	// normalize
	float w = q_in.w;
	float x = q_in.i;
	float y = q_in.j;
	float z = q_in.k;
	float mag = std::sqrt(w * w + x * x + y * y + z * z);
	if (mag > 0.0f) {
		w /= mag; x /= mag; y /= mag; z /= mag;
	}
	else {
		// degenerate, no rotation
		return 0.0f;
	}
	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	return std::atan2(siny_cosp, cosy_cosp); // radians
}

// Create an OBB2D from an Entity to use in collision detection
static OBB2D CreateOBBFromEntity(const Entity& entity) {
	OBB2D obb;
	//Get the center of the entity in 2D space
	obb.center = Vec2(entity.GetPosition().x, entity.GetPosition().y);
	//Get the half extent of the entity
	obb.halfExtents = Vec2(entity.GetHitbox().x * 0.5f, entity.GetHitbox().y * 0.5f);
	//Get the oritentation of the entity in radians
	obb.orientation = Get2DRotationFromQuaternion(entity.GetOrientation());
	//Get the local axes
	obb.xAxis = Vec2(cos(obb.orientation), sin(obb.orientation));
	obb.yAxis = Vec2(-sin(obb.orientation), cos(obb.orientation));
	return obb;
}


static void ProjectOntoAxis(const OBB2D& box, const Vec2& axis, float &outCenterProj, float &outRadius) {
	outCenterProj = box.center.x * axis.x + box.center.y * axis.y; //Projection of the center onto the axis
	outRadius = box.halfExtents.x * fabs(box.xAxis.x * axis.x + box.xAxis.y * axis.y) + // Extent (radius) sum of absolute dot products of half extents onto axis
				box.halfExtents.y * fabs(box.yAxis.x * axis.x + box.yAxis.y * axis.y);
}


bool Collision::OBBvsOBBDetection(const Entity& entity_1, const Entity& entity_2) {
	OBB2D OBB1 = CreateOBBFromEntity(entity_1);
	OBB2D OBB2 = CreateOBBFromEntity(entity_2);
	// Get the 4 axes to test (2 from each box)
	Vec2 axes[4] = {
		OBB1.xAxis,
		OBB1.yAxis,
		OBB2.xAxis,
		OBB2.yAxis
	};

	// Test all axes
	for (auto& axis : axes) {
		axis = VMath::normalize(axis); // Ensure axis is a unit vector
		//if the axis is a zero vector, skip it
		if (VMath::mag(axis) < VERY_SMALL) {
			continue;
		}
		float centerProj1, radius1, centerProj2, radius2;
		ProjectOntoAxis(OBB1, axis, centerProj1, radius1);
		ProjectOntoAxis(OBB2, axis, centerProj2, radius2);
		//Check for overlap
		if ((centerProj1 + radius1) < (centerProj2 - radius2) || (centerProj1 - radius1) > (centerProj2 + radius2)) {
			//No overlap on this axis, so no collision
			return false;
		}
	}
	//Overlap on all axes, so collision
	return true;
}