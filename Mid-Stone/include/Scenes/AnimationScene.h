#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H
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

class AnimationScene : public Scene {
private:

public:
	explicit AnimationScene();
	virtual ~AnimationScene();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
	virtual void RenderGUI();
};


#endif // SCENE0_H