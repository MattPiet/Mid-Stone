#pragma once
#include <Managers/Actor.h>

class Entity;
class Shader;
class SpriteMesh;
class SpriteRenderer;
class AnimationClip;
class Animator;

class Actor2D : public Actor
{
private:

	Entity* entity;
	Shader* SpriteShader;
	Shader* HitboxShader;
	SpriteMesh* sprite_Mesh;
	SpriteRenderer* sprite_Renderer;
	Animator* animator;

public:
	bool draw_Hitbox = false;
	Actor2D();
	virtual ~Actor2D();
	bool OnCreate(const char* FileName, int rows = NULL, int columns = NULL);
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	virtual void Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const override;

	void AddClip(const std::string& name, const AnimationClip* clip) const ;
	
	void ReBuildAll(const char* FileName, int rows = NULL, int columns = NULL);

	Entity* getEntity() const { return entity; }
	SpriteMesh* getMesh() const { return sprite_Mesh; }
	SpriteRenderer* getRenderer() { return sprite_Renderer; }
	Animator* getAnimator() const { return animator; }


};
