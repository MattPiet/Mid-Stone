#pragma once
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
class Mesh;
class Shader;
class SpriteMesh;
class SpriteRenderer;
class Actor2D;
class UIManager;

class MainMenu : public Scene
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

    /** Entity containers **/
    std::vector<std::unique_ptr<Actor2D>> actors;

    std::vector<std::unique_ptr<Actor2D>> objects;

    // For the Background
    std::unique_ptr<Actor2D> backgroundActor;

    // For the png buttons
    std::unique_ptr<Actor2D> mainMenuButton;
    std::unique_ptr<Actor2D> loadButton;
    std::unique_ptr<Actor2D> muteButton;
    std::unique_ptr<Actor2D> quitButton;


    Mesh* bulletMesh;
    Matrix4 BulletModelMatrix;
    

    // For Button States
    bool isDraggingVolume = false;
    float buttonHoverAlpha = 0.8f;  // Alpha for hover effect

    /** Camera **/
    std::unique_ptr<Camera> camera;
    std::unique_ptr<CameraController> cameraController;

    ImFont* MainFont;
	
	bool PlayAudio = true;
	bool ShowLevelSelect = false;
public:
    explicit MainMenu();
    virtual ~MainMenu();

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;
    virtual void HandleEvents(const SDL_Event& sdlEvent) override;
    virtual void RenderGUI() override;


};
