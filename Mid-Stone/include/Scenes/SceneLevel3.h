#pragma once
#include <memory>
#include <queue>

#include "Scene.h"
#include "Graphics/CameraController.h"
#include "Managers/PlayerController.h"
#include <UI/UIManager.h>

class SceneLevel3 : public Scene
{
private:
    /** Main Player **/
    std::unique_ptr<PlayerController> mainPlayerController;
    std::unique_ptr<Actor2D> mainPlayerActor;

    /** Camera **/
    std::unique_ptr<Camera> camera;
    std::unique_ptr<CameraController> cameraController;

    /** Input System **/
    bool leftPressed = false;
    bool rightPressed = false;

    /** Actor Managers **/
    std::vector<std::unique_ptr<Actor2D>> actors;
    std::vector<std::unique_ptr<Actor2D>> terrainActors;
    std::vector<std::unique_ptr<Actor2D>> targets;

    /** Entity Spawn Queue **/
    std::queue<std::unique_ptr<Actor2D>> spawnQueue;
    
    std::vector<std::unique_ptr<Actor2D>> impacts;
    bool canShoot = true;

    /** Audio **/
    MIX_Mixer* mixer = nullptr;
    float master_volume = 1.0f;
	bool PauseAudio = true;
public:
    explicit SceneLevel3();
    virtual ~SceneLevel3();

    /** Scene Lifecycle Methods **/
    void Render() const override;
    void Update(const float deltaTime) override;
    void HandleEvents(const SDL_Event& sdlEvent) override;
    bool OnCreate() override;
    void RenderGUI() override;
    void OnDestroy() override;

    /** Gameplay Functions **/
    void PlayerShoot();
    
};
