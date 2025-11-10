#pragma once
#include <memory>

#include "Scene.h"
#include "Graphics/CameraController.h"
#include "Managers/PlayerController.h"

class SceneLevel1 : public Scene
{
private:
    /** Main Player **/
    std::unique_ptr<PlayerController> mainPlayerController;
    std::unique_ptr<Actor2D> mainPlayerActor;

    /** Camera **/
    std::unique_ptr<Camera> camera;
    std::unique_ptr<CameraController> cameraController;

    /** Input System **/
    bool leftPressed = true;
    bool rightPressed = true;
    
public:
    explicit SceneLevel1();
    virtual ~SceneLevel1();

    /** Scene Class Methods **/
    void Render() const override;
    void Update(const float deltaTime) override;
    void HandleEvents(const SDL_Event& sdlEvent) override;
    bool OnCreate() override;
    void RenderGUI() override;
    void OnDestroy() override;
    
};
