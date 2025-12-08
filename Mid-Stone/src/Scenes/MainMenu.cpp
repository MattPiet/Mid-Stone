#include <glew.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <Scenes/MainMenu.h>
#include <MMath.h>
#include <Utils/Debug.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Graphics/SpriteMesh.h>
#include <SDL_mixer.h>
#include <Graphics/SpriteRenderer.h>
#include <glm/gtx/string_cast.hpp>
#include "Graphics/CameraController.h"
#include "Graphics/Mesh.h"


///ImGui includes
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <Utils/MemoryMonitor.h>


MainMenu::MainMenu() : 
                     drawInWireMode{false}, master_volume{1.0f}, mixer{nullptr} 
{
    Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

MainMenu::~MainMenu()
{
    Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool MainMenu::OnCreate()
{
    Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	PlayAudio = SceneManager::GetAudioStateStatic();
	master_volume = SceneManager::GetMasterVolumeStatic();
    /** Initiate Libraries **/
    SDL_Init(SDL_INIT_AUDIO);
    MIX_Init();
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer)
    {
        std::cout << "Failed to create mixer: %s\n", SDL_GetError();
        return false;
    }

   // Create and load background image
    backgroundActor = std::make_unique<Actor2D>();
    if (!backgroundActor->OnCreate("sprites/MainMenu/MainMenuNoWidgetsTileSheet.png", 1, 10))
    {
        std::cout << "Failed to load menu background image. Using fallback color.\n";
    }
    
    const auto mainMenuClip = new AnimationClip(
        AnimationClip::Play_mode::loop,
        0.15f,
        1, 10,
        0, 9
    );

    backgroundActor->GetAnimator()->AddDefaultAnimationClip("Idle", mainMenuClip);
    backgroundActor->GetAnimator()->PlayAnimationClip("Idle");

    backgroundActor->isStatic = true;

    backgroundActor->GetEntity()->SetPosition(Vec3(0.0f, 0.0f, -1.0f));
    backgroundActor->GetEntity()->SetScale(Vec3(8.5f, 8.5f, 1.0f));


    // Load Level Button
    loadButton = std::make_unique<Actor2D>();
    if (!loadButton->OnCreate("sprites/MainMenu/LoadGame.png")) {
        std::cout << "Failed to load load button image\n";
    }
    loadButton->GetEntity()->SetPosition(Vec3(0.0f, -25.0f, 0.0f));
    loadButton->GetEntity()->SetScale(Vec3(1.2f, 1.2f, 1.2f));
    loadButton->isStatic = true;

    // Mute Button
    muteButton = std::make_unique<Actor2D>();
    if (!muteButton->OnCreate("sprites/MainMenu/MuteGame.png")) {
        std::cout << "Failed to load mute button image\n";
    }
    muteButton->GetEntity()->SetPosition(Vec3(-30.0f, -25.0f, 0.0f));
    muteButton->GetEntity()->SetScale(Vec3(1.2f, 1.2f, 1.2f));
    muteButton->isStatic = true;

    // Quit Button
    quitButton = std::make_unique<Actor2D>();
    if (!quitButton->OnCreate("sprites/MainMenu/QuitGame.png")) {
        std::cout << "Failed to load quit button image\n";
    }
    quitButton->GetEntity()->SetPosition(Vec3(30.0f, -25.0f, 0.0f));
    quitButton->GetEntity()->SetScale(Vec3(1.2f, 1.2f, 1.2f));
    quitButton->isStatic = true;


    /** Load and play music **/
    MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/CrabRave.wav", true);
    MIX_SetMasterGain(mixer, master_volume);
    MIX_PlayAudio(mixer, Music);
    MIX_DestroyAudio(Music);

    /** Camera **/
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());
	ImGuiIO& io = ImGui::GetIO(); // access the ImGuiIO structure
	io.Fonts->AddFontDefault(); // add default font
    
	MainFont = io.Fonts->AddFontFromFileTTF("Fonts/FightingSpirit2Bold.otf", 18.5f); // load a custom font

    return true;
}

void MainMenu::OnDestroy()
{
    Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);

    backgroundActor.reset();
   /* mainMenuButton.reset();*/
    loadButton.reset();
    muteButton.reset();
    quitButton.reset();

    camera.reset();
    cameraController.reset();
    //// Turn off audio
    if (mixer)
    {
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }
}

void MainMenu::HandleEvents(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.type)
    {
    case SDL_EVENT_KEY_DOWN:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_W:
            drawInWireMode = !drawInWireMode;
            break;
        }
        break;

    case SDL_EVENT_KEY_UP:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_W:
            drawInWireMode = !drawInWireMode;

        }
        break;
    case SDL_EVENT_MOUSE_MOTION:
        cameraController->OnMouseMoved({ static_cast<float>(sdlEvent.motion.x), static_cast<float>(sdlEvent.motion.y) });
        break;

    case SDL_EVENT_MOUSE_WHEEL:
    {
        //I KILLED IT FOR THE MAIN MENU ONLY

        /*const float steps = (sdlEvent.wheel.y > 0) ? +1.0f : -1.0f;
        float x, y;
        SDL_GetMouseState(&x, &y);
        cameraController->OnMouseWheel(steps, {x, y});*/
    }
    break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:

        switch (sdlEvent.button.button)
        {
        case SDL_BUTTON_MIDDLE:
            cameraController->OnMouseButtonPressed(2, {
                                                       static_cast<float>(sdlEvent.button.x),
                                                       static_cast<float>(sdlEvent.button.y)
                });
            break;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        switch (sdlEvent.button.button)
        {
        case SDL_BUTTON_MIDDLE:
            cameraController->OnMouseButtonReleased(2);
            break;
        }
        break;
        break;
    default:
        break;
    }
}

void MainMenu::RenderGUI()
{
    /*ImVec4 r = ImVec4(0.984f, 0.949f, 0.212f, 0.6f);*/
    ImVec4 r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
   /* ImVec4 g = ImVec4(1.0f, 0.969f, 0.312f, 0.6f);*/
    ImVec4 g = ImVec4(0.0f, 1.0f, 0.0f, 0.7f);
   /* ImVec4 b = ImVec4(0.884f, 0.849f, 0.112f, 0.9f);*/
    ImVec4 b = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    ImVec4 text = ImVec4(0.95f, 0.95f, 1.0f, 1.0f); // very light blue-white

    ImVec4 Rw = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImVec4 Gw = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImVec4 Bw = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    
    UIManager::StartInvisibleWindow("Hidden Window1", ImVec2(520, 750)); // we start an invisible window here

	UIManager::PushButtonStyle(Bw, Gw, Rw, 90.0f); // pushing button style
	UIManager::PushTextStyle(text, 22.0f); // pushing text style
	
    if (ImGui::Button(" ", ImVec2(250,60))) { // making a button to toggle hitbox drawing
        PlayAudio = !PlayAudio;
		Scene::RequestChangeAudioState(PlayAudio);
    }
    
    if (!PlayAudio)  MIX_SetMasterGain(mixer, 0);
	if (PlayAudio) MIX_SetMasterGain(mixer, master_volume);

    UIManager::PushSliderStyle(b, g, r, 90.0f); // pushing slider style

    if (mixer && PlayAudio)
    {
        ImGui::SliderFloat("Master Volume", &master_volume, 0.0f, 1.0f);
		SceneManager::SetMasterVolumeStatic(master_volume);
    }
	UIManager::PopSliderStyle(); // popping slider style
	UIManager::PopButtonStyle(); // popping button style
	UIManager::PopTextStyle(); // popping text style
    UIManager::EndWindow(); // end the invisible window

    UIManager::StartInvisibleWindow("Hidden Window2", ImVec2(820, 750)); // we start an invisible window here
    UIManager::PushButtonStyle(Bw, Gw, Rw, 90.0f); // pushing button style
    UIManager::PushTextStyle(text, 20.0f); // pushing text style
    if (ImGui::Button(" ", ImVec2(250, 60))) { // making a button to toggle hitbox drawing
		ShowLevelSelect = !ShowLevelSelect;
    }
    UIManager::PopButtonStyle(); // popping button style
    UIManager::PopTextStyle(); // popping text style
    UIManager::EndWindow(); // end the invisible window


    UIManager::StartInvisibleWindow("Hidden Window3", ImVec2(820, 820)); // we start an invisible window here
    UIManager::PushButtonStyle(b, g, r, 90.0f); // pushing button style
    UIManager::PushTextStyle(text, 20.0f); // pushing text style
	if (ShowLevelSelect) {
        if (ImGui::Button("Level 1", ImVec2(250, 60))) { // making a button to toggle hitbox drawing
            RequestChangeScene(Scene_number::scene_level_1);
        }
        if (ImGui::Button("Level 2", ImVec2(250, 60))) { // making a button to toggle hitbox drawing
            RequestChangeScene(Scene_number::scene_level_2);
        }
        if (ImGui::Button("Level 3", ImVec2(250, 60))) { // making a button to toggle hitbox drawing
            RequestChangeScene(Scene_number::scene_level_3);
        }
    }
    UIManager::PopButtonStyle(); // popping button style
	UIManager::PopTextStyle(); // popping text style
	UIManager::EndWindow(); // end the invisible window

	UIManager::StartInvisibleWindow("Hidden Window4", ImVec2(1120, 750)); // we start an invisible window here
    UIManager::PushTextStyle(text, 20.0f); // pushing text style
    UIManager::PushButtonStyle(Bw, Gw, Rw, 90.0f); // pushing button style
    if (ImGui::Button(" ", ImVec2(250, 60))) { // making a button to toggle hitbox drawing
        RequestQuitApplication();
	}
	UIManager::PopButtonStyle(); // popping button style
	UIManager::PopTextStyle(); // popping text style
	UIManager::EndWindow(); // end the invisible window
}

void MainMenu::Update(const float deltaTime)
{
    backgroundActor->Update(deltaTime);
}

void MainMenu::Render() const
{

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render background first (so it's behind everything)
    if (backgroundActor)
    {
        backgroundActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList(); // Get the background draw list to draw behind all ImGui windows
    drawList->AddText(MainFont, 60.0f, ImVec2(600, 180), IM_COL32(0, 255, 0, 255), "Welcome To Richochet Richard!"); // Draw text with custom font and size

    if (loadButton) loadButton->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    if (muteButton) muteButton->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    if (quitButton) quitButton->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    
    if (drawInWireMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glUseProgram(0);
}
