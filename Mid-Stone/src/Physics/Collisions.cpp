#include "Physics/Collisions.h"

bool Collision::CheckCircleCircleCollision(const Actor2D& entity_1, const Actor2D& entity_2){
	// TODO This is wrong, rework later.
	
	//Check for overlap on all axes
		//Distance between the centers of the circles
	float distance = MATH::VMath::distance(entity_1.GetEntity()->GetPosition(), entity_2.GetEntity()->GetPosition()); //Assuming they are in the same plane
	//Sum of the radii of the circles
	float widthSum = entity_1.GetEntity()->GetHitbox().x + entity_2.GetEntity()->GetHitbox().x;
	float heightSum = entity_1.GetEntity()->GetHitbox().y + entity_2.GetEntity()->GetHitbox().y;
	return distance <= widthSum && distance <= heightSum;
}

bool Collision::CheckQuadQuadCollision(const Actor2D& entity_1, const Actor2D& entity_2) {
	//Testing Quad to Quad collision
	float minx1 = entity_1.GetEntity()->GetPosition().x - entity_1.GetEntity()->GetHitbox().x / 2.0f;
	float maxx1 = entity_1.GetEntity()->GetPosition().x + entity_1.GetEntity()->GetHitbox().x / 2.0f;
	float miny1 = entity_1.GetEntity()->GetPosition().y - entity_1.GetEntity()->GetHitbox().y / 2.0f;
	float maxy1 = entity_1.GetEntity()->GetPosition().y + entity_1.GetEntity()->GetHitbox().y / 2.0f;
	                                                              
	float minx2 = entity_2.GetEntity()->GetPosition().x - entity_2.GetEntity()->GetHitbox().x / 2.0f;
	float maxx2 = entity_2.GetEntity()->GetPosition().x + entity_2.GetEntity()->GetHitbox().x / 2.0f;
	float miny2 = entity_2.GetEntity()->GetPosition().y - entity_2.GetEntity()->GetHitbox().y / 2.0f;
	float maxy2 = entity_2.GetEntity()->GetPosition().y + entity_2.GetEntity()->GetHitbox().y / 2.0f;

	bool overlapx = (minx1 <= maxx2) && (maxx1 >= minx2);
	bool overlapy = (miny1 <= maxy2) && (maxy1 >= miny2);

	return overlapx && overlapy;
}
void Collision::CollisionResponse(Actor2D& entityA, Actor2D& entityB) {
    if (entityA.HasExpired() || entityB.HasExpired()) return;

    // Check for collision
    if (!CheckOBBOBBCollision(entityA, entityB)) {
        entityA.GetEntity()->SetHitboxColor(MATH::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        entityB.GetEntity()->SetHitboxColor(MATH::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        return;
    }

    // Visual feedback: collided
    entityA.GetEntity()->SetHitboxColor(MATH::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    entityB.GetEntity()->SetHitboxColor(MATH::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // Positions
    Vec3 posA = entityA.GetEntity()->GetPosition();
    Vec3 posB = entityB.GetEntity()->GetPosition();
    Vec3 delta = posB - posA;

    float distance = VMath::mag(delta);
    if (distance < 1e-6f) return;

    Vec3 normal = delta / distance;

    // Compute overlap depth roughly from AABB projection along normal (simplified)
    Vec3 sizeA = entityA.GetEntity()->GetHitbox() * 0.5f;
    Vec3 sizeB = entityB.GetEntity()->GetHitbox() * 0.5f;

    float overlap = (sizeA.x + sizeB.x) - std::fabs(VMath::dot(delta, normal));
    if (overlap < 0.0f) overlap = 0.0f;

    // --- Position correction ---
    Vec3 correction = normal * (overlap * 0.5f);
    posA -= correction;
    posB += correction;

	if (!entityA.isStatic) entityA.GetEntity()->SetPosition(posA);

   if(!entityB.isStatic) entityB.GetEntity()->SetPosition(posB);

    // --- Velocity response ---
    Vec3 velA = entityA.GetEntity()->GetVelocity();
    Vec3 velB = entityB.GetEntity()->GetVelocity();

    


    Vec3 relativeVel = velB - velA;
    float separatingVel = VMath::dot(relativeVel, normal);

    // Avoid tiny floating-point overlap errors
    const float overlapEpsilon = 0.0001f;

    // Skip if moving apart AND not overlapping anymore
    if (separatingVel > 0.0f && overlap <= overlapEpsilon)
        return;

    // Prevent small vibrations/jitter due to numerical noise
    if (fabs(separatingVel) < 0.01f)
        separatingVel = 0.0f;
    float restitution = 1.8f; // 0 = no bounce, 1 = perfectly elastic
    // Compute impulse magnitude (elastic collision, equal mass)
    float impulse = -(1.0f + restitution) * separatingVel; // apply restitution
   
    float mA = entityA.GetEntity()->GetMass();
    float mB = entityB.GetEntity()->GetMass();
    float totalMass = mA + mB;
  //  Vec3 impulseVec = normal * (impulse * mB / totalMass); // apply mass ratio
    Vec3 impulseVec = normal * impulse * 0.5f; // no mass


    if (!entityA.isStatic) entityA.GetEntity()->SetVelocity(velA - impulseVec);
    if (!entityB.isStatic) entityB.GetEntity()->SetVelocity(velB + impulseVec);

}

bool Collision::CheckOBBOBBCollision(const Actor2D& boxA, const Actor2D& boxB) {
    // Centers
    Vec3 centerA = boxA.GetEntity()->GetPosition();
    Vec3 centerB = boxB.GetEntity()->GetPosition();

    // Half-extents (half width, half height, half depth)
    Vec3 halfExtentsA = boxA.GetEntity()->GetHitbox() * 0.5f;
    Vec3 halfExtentsB = boxB.GetEntity()->GetHitbox() * 0.5f;

    // Orientations (quaternions)
    Quaternion orientationA = boxA.GetEntity()->GetOrientation();
    Quaternion orientationB = boxB.GetEntity()->GetOrientation();

    // Local axes of each OBB expressed in world space (unit vectors)
    Vec3 axisA[3] = {
        QMath::rotate(MATH::Vec3(1.0f, 0.0f, 0.0f),orientationA),// get the ori of the object based off a quat
		QMath::rotate(MATH::Vec3(0.0f, 1.0f, 0.0f),orientationA),// get the ori of the object based off a quat
		QMath::rotate(MATH::Vec3(0.0f, 0.0f, 1.0f),orientationA) // get the ori of the object based off a quat
    };
    Vec3 axisB[3] = {
		QMath::rotate(MATH::Vec3(1.0f, 0.0f, 0.0f),orientationB), // get the ori of the object based off a quat
		QMath::rotate(MATH::Vec3(0.0f, 1.0f, 0.0f),orientationB), // get the ori of the object based off a quat
		QMath::rotate(MATH::Vec3(0.0f, 0.0f, 1.0f),orientationB)  // get the ori of the object based off a quat
    };

    //  normalize axes
    for (int i = 0; i < 3; ++i) {
        axisA[i] = VMath::normalize(axisA[i]);
        axisB[i] = VMath::normalize(axisB[i]);
    }

    // Rotation / dot-product matrix between A's axes and B's axes:
    // rotationDotMatrix[i][j] = axisA[i] · axisB[j]
    float rotationDotMatrix[3][3];
    float absRotationDotMatrix[3][3];
    const float EPSILON = 1e-5f;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            rotationDotMatrix[i][j] = VMath::dot(axisA[i], axisB[j]);
            absRotationDotMatrix[i][j] = std::fabs(rotationDotMatrix[i][j]) + EPSILON;
        }
    }

    // Translation vector from A to B, expressed in A's local coordinate frame:
    Vec3 translationWorld = centerB - centerA;
    Vec3 translationInA = Vec3(
        VMath::dot(translationWorld, axisA[0]),
        VMath::dot(translationWorld, axisA[1]),
        VMath::dot(translationWorld, axisA[2])
    );

    float radiusA, radiusB;

    // ---------- Test axes: A's local axes (A0, A1, A2) ----------
    // Projection of center distance on Ai is just fabs(translationInA[i])
    for (int i = 0; i < 3; ++i) {
        radiusA = halfExtentsA[i];
        radiusB = halfExtentsB.x * absRotationDotMatrix[i][0]
            + halfExtentsB.y * absRotationDotMatrix[i][1]
            + halfExtentsB.z * absRotationDotMatrix[i][2];
        if (std::fabs(translationInA[i]) > radiusA + radiusB) return false; // separation found
    }

    // ---------- Test axes: B's local axes (B0, B1, B2) ----------
    for (int j = 0; j < 3; ++j) {
        radiusA = halfExtentsA.x * absRotationDotMatrix[0][j]
            + halfExtentsA.y * absRotationDotMatrix[1][j]
            + halfExtentsA.z * absRotationDotMatrix[2][j];
        radiusB = halfExtentsB[j];
        float projectedTranslationOnBj = std::fabs(
            translationInA.x * rotationDotMatrix[0][j]
            + translationInA.y * rotationDotMatrix[1][j]
            + translationInA.z * rotationDotMatrix[2][j]
        );
        if (projectedTranslationOnBj > radiusA + radiusB) return false; // separation found
    }
    //// No separating axis found => boxes overlap
    // ---------- Test cross-product axes (Ai x Bj), 9 tests ----------
// A0 x B0
    radiusA = halfExtentsA.y * absRotationDotMatrix[2][0] + halfExtentsA.z * absRotationDotMatrix[1][0];
    radiusB = halfExtentsB.y * absRotationDotMatrix[0][2] + halfExtentsB.z * absRotationDotMatrix[0][1];
    if (std::fabs(translationInA.z * rotationDotMatrix[1][0] - translationInA.y * rotationDotMatrix[2][0]) > radiusA + radiusB) return false;

    // A0 x B1
    radiusA = halfExtentsA.y * absRotationDotMatrix[2][1] + halfExtentsA.z * absRotationDotMatrix[1][1];
    radiusB = halfExtentsB.x * absRotationDotMatrix[0][2] + halfExtentsB.z * absRotationDotMatrix[0][0];
    if (std::fabs(translationInA.z * rotationDotMatrix[1][1] - translationInA.y * rotationDotMatrix[2][1]) > radiusA + radiusB) return false;

    // A0 x B2
    radiusA = halfExtentsA.y * absRotationDotMatrix[2][2] + halfExtentsA.z * absRotationDotMatrix[1][2];
    radiusB = halfExtentsB.x * absRotationDotMatrix[0][1] + halfExtentsB.y * absRotationDotMatrix[0][0];
    if (std::fabs(translationInA.z * rotationDotMatrix[1][2] - translationInA.y * rotationDotMatrix[2][2]) > radiusA + radiusB) return false;

    // A1 x B0
    radiusA = halfExtentsA.x * absRotationDotMatrix[2][0] + halfExtentsA.z * absRotationDotMatrix[0][0];
    radiusB = halfExtentsB.y * absRotationDotMatrix[1][2] + halfExtentsB.z * absRotationDotMatrix[1][1];
    if (std::fabs(translationInA.x * rotationDotMatrix[2][0] - translationInA.z * rotationDotMatrix[0][0]) > radiusA + radiusB) return false;

    // A1 x B1
    radiusA = halfExtentsA.x * absRotationDotMatrix[2][1] + halfExtentsA.z * absRotationDotMatrix[0][1];
    radiusB = halfExtentsB.x * absRotationDotMatrix[1][2] + halfExtentsB.z * absRotationDotMatrix[1][0];
    if (std::fabs(translationInA.x * rotationDotMatrix[2][1] - translationInA.z * rotationDotMatrix[0][1]) > radiusA + radiusB) return false;

    // A1 x B2
    radiusA = halfExtentsA.x * absRotationDotMatrix[2][2] + halfExtentsA.z * absRotationDotMatrix[0][2];
    radiusB = halfExtentsB.x * absRotationDotMatrix[1][1] + halfExtentsB.y * absRotationDotMatrix[1][0];
    if (std::fabs(translationInA.x * rotationDotMatrix[2][2] - translationInA.z * rotationDotMatrix[0][2]) > radiusA + radiusB) return false;

    // A2 x B0
    radiusA = halfExtentsA.x * absRotationDotMatrix[1][0] + halfExtentsA.y * absRotationDotMatrix[0][0];
    radiusB = halfExtentsB.y * absRotationDotMatrix[2][2] + halfExtentsB.z * absRotationDotMatrix[2][1];
    if (std::fabs(translationInA.y * rotationDotMatrix[0][0] - translationInA.x * rotationDotMatrix[1][0]) > radiusA + radiusB) return false;

    // A2 x B1
    radiusA = halfExtentsA.x * absRotationDotMatrix[1][1] + halfExtentsA.y * absRotationDotMatrix[0][1];
    radiusB = halfExtentsB.x * absRotationDotMatrix[2][2] + halfExtentsB.z * absRotationDotMatrix[2][0];
    if (std::fabs(translationInA.y * rotationDotMatrix[0][1] - translationInA.x * rotationDotMatrix[1][1]) > radiusA + radiusB) return false;

    // A2 x B2
    radiusA = halfExtentsA.x * absRotationDotMatrix[1][2] + halfExtentsA.y * absRotationDotMatrix[0][2];
    radiusB = halfExtentsB.x * absRotationDotMatrix[2][1] + halfExtentsB.y * absRotationDotMatrix[2][0];
    if (std::fabs(translationInA.y * rotationDotMatrix[0][2] - translationInA.x * rotationDotMatrix[1][2]) > radiusA + radiusB) return false;

    return true;
}





