#include "Entities/Entity.h"
#include <Utils/MemoryMonitor.h>

#include <iostream>
#include <MMath.h>
#include <ostream>


Entity::Entity(const MATH::Vec3& position, const MATH::Vec3& scale)
{
    this->position = position;
    this->scale = scale;
    this->hitBoxType = Hit_box_type::quad;
    velocity = Vec3(0.0f, 0.0f, 0.0f), acceleration = Vec3(0.0f, 0.0f, 0.0f), mass = 1.0f;
}

Entity::Entity(const Vec3& position, const Vec3& scale, const Hit_box_type& hitBoxType)
{
    this->position = position;
    this->scale = scale;
	this->hitBoxType = hitBoxType;
    velocity = Vec3(0.0f, 0.0f, 0.0f), acceleration = Vec3(0.0f, 0.0f, 0.0f), mass = 1.0f;
}

void Entity::OnCreate(SpriteRenderer* renderer) {
    shader = new Shader("shaders/HitboxVert.glsl", "shaders/HitboxFrag.glsl");
    if (!shader->OnCreate())
    {
        std::cout << "Hitbox Shader failed ... we have a problem\n";
    }
    if (renderer->GetColumns() == NULL){
        CreateHitBox(renderer);
    }
    else{
		CreateHitBox(renderer, 0);
    }
}

void Entity::CreateHitBox(SpriteRenderer* renderer){
        std::pair desiredWidth_Height = renderer->buildSprite();
        hitbox = MATH::Vec3(desiredWidth_Height.first * scale.x, desiredWidth_Height.second * scale.y, 1.0f);
}
void Entity::CreateHitBox(SpriteRenderer* renderer, int CurrentIndex) {
        std::pair desiredWidth_Height = renderer->buildSpriteSheet(CurrentIndex);
        hitbox = MATH::Vec3(desiredWidth_Height.first * scale.x, desiredWidth_Height.second * scale.y, 1.0f);
}
void Entity::OnDestroy(){
    std::cout << "Im Destroying My shader\n";
    shader->OnDestroy();
    delete shader;
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

void Entity::UpdatePhysics(float deltaTime)
{
    position += velocity * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
    velocity += acceleration * deltaTime;
	
}

void Entity::ApplyForce(Vec3 force) {
	acceleration = force / mass;
}

void Entity::DrawHitBox(MATH::Matrix4 projectionMatrix, MATH::Matrix4 viewMatrix, SpriteMesh* mesh)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (shader) {
        glUseProgram(shader->GetProgram());
        glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
        Matrix4 model = MMath::translate(position) *
                        MMath::toMatrix4(orientation) *
			            MMath::scale(MATH::Vec3(hitbox.x, hitbox.y, 1.0f));
        glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, model);
        glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
		glUniform4fv(shader->GetUniformID("hitboxColor"), 1, hitboxColor);
        mesh->RenderMesh();
    }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUseProgram(0);
}
