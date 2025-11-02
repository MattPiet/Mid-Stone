#pragma once
#include <functional>
#include <Matrix.h>
#include <Quaternion.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Graphics/Shader.h>
#include <Graphics/SpriteMesh.h>
#include <Graphics/SpriteRenderer.h>
#include <Entities/Entity.h>
#include <Graphics/Animator.h>
#include <Graphics/AnimationClip.h>

class Actor
{
public:
	virtual ~Actor() {}
	//virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void Update(const float deltaTime_) = 0;
	virtual void Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const = 0;
};
