#include "Entities/Entity.h"
#include <Utils/MemoryMonitor.h>

#include <iostream>
#include <MMath.h>
#include <ostream>


Entity::Entity(const Vec3& position, const Vec3& scale, const char& hitBoxType)
{
    this->position = position;
    this->scale = scale;
	this->hitBoxType = hitBoxType;
}

void Entity::OnCreate(SpriteRenderer* renderer){
    shader = new Shader("shaders/hitboxVert.glsl", "shaders/hitboxFrag.glsl");
    if (!shader->OnCreate())
    {
    std::cout << "Hitbox Shader failed ... we have a problem\n";
    }
    if (renderer->GetRows() > 0 && renderer->GetColumns() > 0) { // if its a spritesheet
        float frameWidth = (float)renderer->GetImageWidth() / renderer->GetColumns();
        float frameHeight = (float)renderer->GetImageHeight() / renderer->GetRows();
        float aspect = frameWidth / frameHeight;
        float desiredHeight = 64.0f / 5.0f;
        float desiredWidth = desiredHeight * aspect;
        hitbox = MATH::Vec3(desiredWidth, desiredHeight, 1.0f);
    }
    else { // if its not a spritesheet
        float aspect = (float)renderer->GetImageWidth() / renderer->GetImageHeight();
        float desiredHeight = 64.0f / 10.0f;
        float desiredWidth = desiredHeight * aspect;
        hitbox = MATH::Vec3(desiredWidth, desiredHeight, 1.0f);
    }
    hitbox = hitbox * 0.5f; // slightly bigger hitbox for better visibility
	hitbox.x *= scale.x;
    hitbox.y *= scale.y;
}

void Entity::OnDestroy(){
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

void Entity::DrawHitBox(MATH::Matrix4 projectionMatrix, SpriteMesh* mesh)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (shader) {
        glUseProgram(shader->GetProgram());
        glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
        glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, this->GetModelMatrix());
        glUniform3fv(shader->GetUniformID("scale"), 1, hitbox);
		glUniform4fv(shader->GetUniformID("hitboxColor"), 1, hitboxColor);
        mesh->RenderMesh();
    }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUseProgram(0);
}
