#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <SDL_mixer.h>
#include <memory>
#include <queue>


#include "Entities/Entity.h"
#include "Entities/Player.h"
#include "Graphics/Camera.h"
#include "Graphics/CameraController.h"
#include <Managers/2DActor.h>
#include <UI/UIManager.h>

#include "Managers/PlayerController.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class SpriteMesh;
class SpriteRenderer;
class AnimationClip;
class Animator;
class Actor2D;
class UIManager;

class Scene0g : public Scene
{
private:
    /** Scene Components **/
    MIX_Mixer* mixer;

    /** Scene Features **/
    bool drawInWireMode;
    float master_volume = 1.0f;

    /** Input System **/
    bool pressingLeft = false;
    bool pressingRight = false;

    /** Camera **/
    std::unique_ptr<Camera> camera;
    std::unique_ptr<CameraController> cameraController;

    /** Player Controller **/
    std::unique_ptr<PlayerController> mainPlayerController;
    std::unique_ptr<Actor2D> mainPlayerActor;

    /** Actor Containers **/
    std::vector<std::unique_ptr<Actor2D>> visualEffects;


    /** Entity containers **/
    std::vector<std::unique_ptr<Actor2D>> actors;

	std::vector<std::unique_ptr<Actor2D>> objects;

    ImFont* MainFont;

    /** Entity Spawn Queue **/
    std::queue<std::unique_ptr<Actor2D>> spawnQueue;
	
	bool PauseAudio = true;

    bool globalDrawHitboxes = false;

public:
    explicit Scene0g();
    virtual ~Scene0g();

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;
    virtual void HandleEvents(const SDL_Event& sdlEvent) override;
    virtual void RenderGUI() override;


    /** Gameplay Functions **/
    void PlayerShoot();
};


#endif // SCENE0_H
