#include <SDL.h>

#include <Core/SceneManager.h>
#include <Utils/Debug.h>
#include <Core/Timer.h>
#include <Core/Window.h>
#include <UI/GuiWindow.h>
#include <Scenes/Scene0g.h>
#include <Scenes/AnimationScene.h>
#include <Scenes/MainMenu.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <UI/UIManager.h>

#include "Scenes/SceneLevel1.h"
<<<<<<< HEAD
#include "Scenes/SceneLevel3.h"
=======

bool SceneManager::setAudioOn = true;
float SceneManager::master_volume = 1.0f;
>>>>>>> master

SceneManager::SceneManager() :
    currentScene{nullptr},
    window{nullptr},
    imguiWin{nullptr},
    timer{nullptr},
    fps(144),
    isRunning{false},
    fullScreen{false}
{
    Debug::Info("Starting the SceneManager", __FILE__, __LINE__);
}

SceneManager::~SceneManager()
{
    Debug::Info("Deleting the SceneManager", __FILE__, __LINE__);

    if (currentScene)
    {
        currentScene->OnDestroy();
        delete currentScene;
        currentScene = nullptr;
    }

    if (timer)
    {
        delete timer;
        timer = nullptr;
    }

    if (window)
    {
        delete window;
        window = nullptr;
    }

    if (imguiWin)
    {
        delete imguiWin;
        imguiWin = nullptr;
    }
}

bool SceneManager::Initialize(std::string name_, int width_, int height_)
{
    bool useImGui = true; // flip this to false if you want to use Window instead

    if (useImGui)
    {
        imguiWin = new GuiWindow();
        if (!imguiWin->OnCreate(name_, width_, height_))
        {
            Debug::FatalError("Failed to initialize ImGuiWindow object", __FILE__, __LINE__);
            return false;
        }
    }
    else
    {
        window = new Window();
        if (!window->OnCreate(name_, width_, height_))
        {
            Debug::FatalError("Failed to initialize Window object", __FILE__, __LINE__);
            return false;
        }
    }

    timer = new Timer();
    if (!timer)
    {
        Debug::FatalError("Failed to initialize Timer object", __FILE__, __LINE__);
        return false;
    }

    /********************************   Default first scene   ***********************/
<<<<<<< HEAD
    BuildNewScene(Scene_number::scene_level_3);
=======
    BuildNewScene(Scene_number::MainMenu);
>>>>>>> master
    /********************************************************************************/
    return true;
}

void SceneManager::Run()
{
    timer->Start();
    isRunning = true;

    while (isRunning)
    {
        HandleEvents();
        timer->UpdateFrameTicks();

        if (imguiWin)
        {
            imguiWin->BeginFrame();
        }

        // Scene logic
        currentScene->Update(timer->GetDeltaTime());
        currentScene->Render();

        // --- Scene-specific GUI ---
        if (imguiWin && currentScene)
        {
            currentScene->RenderGUI(); // <-- new per-scene ImGui
        }

        // --- Global debug GUI ---
        if (imguiWin)
        {
            ImVec4 r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            ImVec4 g = ImVec4(0.0f, 1.0f, 0.0f, 0.7f);
            ImVec4 b = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
            ImVec4 text = ImVec4(0.95f, 0.95f, 1.0f, 1.0f); // very light blue-white

            UIManager::StartInvisibleWindow("Hidden WindowSceneManager", ImVec2(1845, 0)); // we start an invisible window here
            UIManager::PushButtonStyle(b, g, r, 90.0f); // pushing button style
            UIManager::PushTextStyle(text, 20.0f); // pushing text style
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("FPS: %1.f", io.Framerate);
            if (ImGui::Button("MainMenu"))
            {
				BuildNewScene(Scene_number::MainMenu);
            }
     
			UIManager::PopButtonStyle(); // popping button style
			UIManager::PopTextStyle(); // popping text style
            UIManager::EndWindow(); // end the invisible window
        }

        if (imguiWin)
        {
            imguiWin->EndFrame();
        }
        else if (window)
        {
            SDL_GL_SwapWindow(window->GetWindow());
        }

        HandleSceneRequest();

        SDL_Delay(timer->GetSleepTime(fps));
    }
}

void SceneManager::HandleEvents()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        // Let ImGui process events if it's enabled
        if (imguiWin)
        {
            ImGui_ImplSDL3_ProcessEvent(&sdlEvent);
        }

        if (sdlEvent.type == SDL_EVENT_QUIT)
        {
            isRunning = false;
            return;
        }
        else if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
        {
            switch (sdlEvent.key.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
            case SDL_SCANCODE_Q:
                isRunning = false;
                return;

            case SDL_SCANCODE_F1:
                DestroyScene();
                BuildNewScene(Scene_number::scene0_g);
                break;
            case SDL_SCANCODE_F2:
                DestroyScene();
                BuildNewScene(Scene_number::animation_scene);
                break;
            case SDL_SCANCODE_F3:
                DestroyScene();
                BuildNewScene(Scene_number::scene_level_1);
            case SDL_SCANCODE_F4:
            case SDL_SCANCODE_F5:
                break;

            default:
                break;
            }
        }

        if (currentScene == nullptr)
        {
            Debug::FatalError("No currentScene", __FILE__, __LINE__);
            isRunning = false;
            return;
        }
        currentScene->HandleEvents(sdlEvent);
    }
}

bool SceneManager::BuildNewScene(Scene_number scene)
{
    bool status;

    if (currentScene != nullptr)
    {
        currentScene->OnDestroy();
        delete currentScene;
        currentScene = nullptr;
    }

    currentSceneId = scene;

    switch (scene)
    {
    case Scene_number::scene0_g:
        currentScene = new Scene0g();
        status = currentScene->OnCreate();
        break;

    case Scene_number::animation_scene:
        currentScene = new AnimationScene();
        status = currentScene->OnCreate();
        break;

    case Scene_number::scene_level_1:
        currentScene = new SceneLevel1();
        status = currentScene->OnCreate();
        break;
	case Scene_number::MainMenu:
		currentScene = new MainMenu();
		status = currentScene->OnCreate();
		break;

    case Scene_number::scene_level_3:
        currentScene = new SceneLevel3();
        status = currentScene->OnCreate();
        break;
    default:
        Debug::Error("Incorrect scene number assigned in the manager", __FILE__, __LINE__);
        currentScene = nullptr;
        return false;
    }
    return status;
}

void SceneManager::DestroyScene()
{
    if (currentScene)
    {
        currentScene->OnDestroy();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::HandleSceneRequest()
{
    if (currentScene == nullptr)
    {
        return;
    }

    SceneRequest request = currentScene->ConsumeSceneRequest();

    switch (request.type)
    {
    case Scene_request_type::none:
        break;

    case Scene_request_type::quit_application:
        isRunning = false;
        break;

    case Scene_request_type::restart_scene:
        BuildNewScene(currentSceneId);
        break;

    case Scene_request_type::change_scene:
        BuildNewScene(request.targetScene);
        break;
	case Scene_request_type::change_audio_state:
		setAudioOn = !setAudioOn;
        break;
    }
}

