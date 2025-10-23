#include "Entities/Entity.h"

#include <iostream>
#include <MMath.h>
#include <ostream>

Entity::Entity(const Vec3& position, const Vec3& scale)
{
    this->position = position;
    this->scale = scale;
}

void Entity::SetLifeSpan(float lifeSpanSeconds_)
{
    lifeSpanSeconds = lifeSpanSeconds_;
    currentLifeTimeSeconds = 0.0f; // restart the timer whenever lifespan is (re)set
    hasFiredExpiredHooks = false;
}

bool Entity::IsExpired() const
{
    return lifeSpanSeconds > 0.0f && currentLifeTimeSeconds >= lifeSpanSeconds;
}


MATH::Matrix4 Entity::GetModelMatrix() const
{
    // Model matrix sets the scale, rotation, and position of the mesh
    const MATH::Matrix4 translation = MMath::translate(position);
    const MATH::Matrix4 rotation = MMath::toMatrix4(orientation);
    const MATH::Matrix4 modelScale = MMath::scale(scale);
    return translation * rotation * modelScale;
}

void Entity::Update(float deltaTime)
{
    if (lifeSpanSeconds > 0.0f)
    {
        currentLifeTimeSeconds += std::max(0.0f, deltaTime);
        if (!hasFiredExpiredHooks && currentLifeTimeSeconds >= lifeSpanSeconds)
        {
            hasFiredExpiredHooks = true;
            // Class Callback
            OnExpired();

            // External Callback
            if (onExpired) { onExpired(*this); }
        }
    }
}
void Entity::ApplyForce(MATH::Vec3 force) {
    acceleration = force / mass;
}

void Entity::UpdatePosition(float deltaTime) {
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
}
