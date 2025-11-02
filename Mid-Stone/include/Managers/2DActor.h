#pragma once
#include <Managers/Actor.h>

class Entity;
class Shader;
class SpriteMesh;
class SpriteRenderer;
class AnimationClip;
class Animator;

class ActorTwoD : public Actor
{
private:

	Entity* entity;
	Shader* SpriteShader;
	Shader* HitboxShader;
	SpriteMesh* sprite_Mesh;
	SpriteRenderer* sprite_Renderer;
	Animator* animator;

public:
	explicit ActorTwoD();
	virtual ~ActorTwoD();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	virtual void Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const override;

};
