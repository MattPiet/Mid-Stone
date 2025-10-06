#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>

#include <SDL_mixer.h>

using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class SpriteMesh;
class SpriteRenderer;

class Scene0g : public Scene {
private:
	Body* sphere;
	Mesh* mesh;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 modelMatrix;
	bool drawInWireMode;
	float master_volume = 1.0f;
	MIX_Mixer* mixer;

	/// / This is everything we need for sprite rendering
	Shader* shader;
	Matrix4 spriteProjectionMatrix;
	// you need 1 sprite mesh and 1 sprite renderer to render a single sprite 
	SpriteMesh* sprite_Mesh;
	SpriteRenderer* sprite_Renderer;

	SpriteRenderer* spriteSheet_Renderer;

	Matrix4 spriteSheet_ModelMatrix;


public:
	explicit Scene0g();
	virtual ~Scene0g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	virtual void RenderGUI();
};


#endif // SCENE0_H