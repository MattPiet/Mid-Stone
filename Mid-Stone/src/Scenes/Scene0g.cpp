#include <glew.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <Scenes/Scene0g.h>
#include <MMath.h>
#include <Utils/Debug.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Entities/Body.h>
#include <Graphics/SpriteMesh.h>
#include <SDL_mixer.h>
#include <Graphics/SpriteRenderer.h>
#include <glm/gtx/string_cast.hpp>
#include <Physics/Collisions.h>

#include <Graphics/Animator.h>

///ImGui includes
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Utils/MemoryMonitor.h>


Scene0g::Scene0g() : 
                     drawInWireMode{false}, master_volume{1.0f}, mixer{nullptr}
{
    Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0g::~Scene0g()
{
    Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene0g::OnCreate()
{
    Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);

    /** Initiate Libraries **/
    SDL_Init(SDL_INIT_AUDIO);
    MIX_Init();
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer)
    {
        std::cout << "Failed to create mixer: %s\n", SDL_GetError();
        return false;
    }

    /** Load and play music **/
    MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/CrabRave.wav", true);
    MIX_SetMasterGain(mixer, master_volume);
    MIX_PlayAudio(mixer, Music);
    MIX_DestroyAudio(Music);

    /** Camera **/
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());

    /** Set up Player **/
    mainPlayerActor = std::make_unique<Actor2D>();
    if (!mainPlayerActor->OnCreate("sprites/Idle.png", 1, 3))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
	mainPlayerActor->LowPosistionCorrection = true;
    // TODO Why do i set rows and columns twice my n word
    auto mainPlayerClipIdle = new AnimationClip(
        AnimationClip::PlayMode::PINGPONG,
        0.1f,
        1, 3
    );
    // TODO, The clip not being directly related to the spritesheet is weird no?
    mainPlayerActor->GetAnimator()->addAnimationClip("Idle", mainPlayerClipIdle);
    mainPlayerActor->GetAnimator()->playAnimationClip("Idle");
    //mainPlayerActor->draw_Hitbox = true;

    /** Set up Main Player Controller **/
    mainPlayerController = std::make_unique<PlayerController>();

    mainPlayerController->OnCreate("sprites/crosshairs.png");
    mainPlayerController->SetPossessedActor(mainPlayerActor.get());



	ImGuiIO& io = ImGui::GetIO(); // access the ImGuiIO structure
	io.Fonts->AddFontDefault(); // add default font

  
    // we can use this to draw stuff striaght to the screen using ImGui
    
	MainFont = io.Fonts->AddFontFromFileTTF("Fonts/times.ttf", 18.5f); // load a custom font

	// Spawn Test Objects
    // top left
    auto NewOBJ = std::make_unique<Actor2D>();
    NewOBJ->OnCreate("sprites/impact.png");
    NewOBJ->GetEntity()->SetPosition(Vec3(-50.0f,10.0f,0.0f));
	NewOBJ->isStatic = true;
    objects.emplace_back(std::move(NewOBJ));
    // top right
    NewOBJ = std::make_unique<Actor2D>();
   NewOBJ->OnCreate("sprites/impact.png");
   NewOBJ->GetEntity()->SetPosition(Vec3(0.0f, -10.0f, 0.0f));
   NewOBJ->isStatic = true;
   objects.emplace_back(std::move(NewOBJ));
   // bottom left
    NewOBJ = std::make_unique<Actor2D>();
   NewOBJ->OnCreate("sprites/impact.png");
   NewOBJ->GetEntity()->SetPosition(Vec3(-50.0f, 0.0f, 0.0f));
   NewOBJ->isStatic = true;
   objects.emplace_back(std::move(NewOBJ));
   // bottom right
    NewOBJ = std::make_unique<Actor2D>();
   NewOBJ->OnCreate("sprites/impact.png");
   NewOBJ->GetEntity()->SetPosition(Vec3(50.0f, 0.0f, 0.0f));
   NewOBJ->isStatic = true;
   objects.emplace_back(std::move(NewOBJ));

    return true;
}

void Scene0g::OnDestroy()
{
    Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
    /** Delete Main Player **/
    mainPlayerActor.reset();
    mainPlayerController.reset();

    cameraController.reset();
    camera.reset();

    for (auto& actor : actors)
	{
		actor.reset();
	}
    for (auto& object : objects)
    {
        object.reset();
	}

    //// Turn off audio
    if (mixer)
    {
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }
}
void Scene0g::PlayerShoot() {
    const Vec3 currentCrossHairsPosition = mainPlayerController->GetCrossHairsPosition();
    auto bullet = std::make_unique<Actor2D>();
    bullet->OnCreate("sprites/fatty_clicked.png");
    bullet->GetEntity()->SetPosition(currentCrossHairsPosition);
    const auto crossHairsQuaternion = mainPlayerController->GetCrossHairsRotation();
    const auto forward = Vec3(1.0f, 0.0f, 0.0f);
    const Vec3 rotatedVector = crossHairsQuaternion * forward * QMath::inverse(crossHairsQuaternion); // QPQ-1 Formula
    const Vec3 finalVelocity = rotatedVector * 100.0f;
    bullet->GetEntity()->AdjustOrientation(crossHairsQuaternion);
    bullet->GetEntity()->SetVelocity(finalVelocity);
    bullet->ConfigureLifeSpan(2.0f);
	//bullet->draw_Hitbox = globalDrawHitboxes;
    bullet->RegisterExpiredCallback([this](const Actor2D& actor)
        {
            auto impact = std::make_unique<Actor2D>();
            impact->OnCreate("sprites/impact.png");
            impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
            impact->ConfigureLifeSpan(1.0f);
			//impact->draw_Hitbox = globalDrawHitboxes;
            spawnQueue.emplace(std::move(impact));
        });
    spawnQueue.emplace(std::move(bullet));

}

void Scene0g::HandleEvents(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.type)
    {
    case SDL_EVENT_KEY_DOWN:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_W:
            drawInWireMode = !drawInWireMode;
            break;
        case SDL_SCANCODE_A:
            pressingLeft = true;
            break;
        case SDL_SCANCODE_D:
            pressingRight = true;
            break;
        case SDL_SCANCODE_SPACE:
            PlayerShoot();
            break;
        case SDL_SCANCODE_P:
            break;
        }
        break;

    case SDL_EVENT_KEY_UP:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_W:
            drawInWireMode = !drawInWireMode;
            break;
        case SDL_SCANCODE_A:
            pressingLeft = false;
            break;
        case SDL_SCANCODE_D:
            pressingRight = false;
            break;
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        cameraController->OnMouseMoved({static_cast<float>(sdlEvent.motion.x), static_cast<float>(sdlEvent.motion.y)});
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        {
            const float steps = (sdlEvent.wheel.y > 0) ? +1.0f : -1.0f;
            float x, y;
            SDL_GetMouseState(&x, &y);
            cameraController->OnMouseWheel(steps, {x, y});
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

void Scene0g::RenderGUI()
{
    ImVec4 r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 g = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    ImVec4 b = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    
    UIManager::StartInvisibleWindow(ImVec2(0, 100)); // we start an invisible window here

	UIManager::PushButtonStyle(b, g, r, 90.0f); // pushing button style

    if (ImGui::Button("DrawHitBox")) { // making a button to toggle hitbox drawing
        mainPlayerActor->draw_Hitbox = !mainPlayerActor->draw_Hitbox;
		globalDrawHitboxes = !globalDrawHitboxes;
        for (auto& object : objects)
        {
            object->draw_Hitbox = globalDrawHitboxes;
		}
     
    }
	
    if (ImGui::Button("Mute Audio")) { // making a button to toggle hitbox drawing
        PauseAudio = !PauseAudio;
    }
    
    if (PauseAudio)  MIX_SetMasterGain(mixer, 0);
	if (!PauseAudio) MIX_SetMasterGain(mixer, master_volume);

    UIManager::PushSliderStyle(b, g, r, 90.0f); // pushing slider style

    if (mixer && !PauseAudio)
    {
        ImGui::SliderFloat("Master Volume", &master_volume, 0.0f, 1.0f);
    }
	UIManager::PopSliderStyle(); // popping slider style
	UIManager::PopButtonStyle(); // popping button style

    UIManager::EndWindow(); // end the invisible window
}

void Scene0g::Update(const float deltaTime)
{
	Vec3 Gravity = Vec3(0.0f, -9.81f, 0.0f);
    
    /** Spawn Queue **/
    while (!spawnQueue.empty())
    {
        actors.emplace_back(std::move(spawnQueue.front()));
        spawnQueue.pop();
    }
   

    /** Update Main Player **/
	mainPlayerActor->GetEntity()->ApplyForce(Gravity * mainPlayerActor->GetEntity()->GetMass());
    mainPlayerActor->Update(deltaTime);

    /** Update Players **/
    if (pressingLeft && !pressingRight)
    {
        mainPlayerController->MoveAim(1.0f);
    }
    if (pressingRight && !pressingLeft)
    {
        mainPlayerController->MoveAim(-1.0f);
    }

    /* Actor Cleanup */
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
            [](const std::unique_ptr<Actor2D>& e) { return e->HasExpired(); }),
        actors.end()
    );

    /** Update Actors **/
    for (const auto& actor : actors)
    {
        actor->Update(deltaTime);
    }



    /* Removes and Deletes bullets if they expire */
    // TODO Delete after demo.
 //   for (auto& object : objects)
 //   {
 //       object->Update(deltaTime);
	//}
    for (auto& actor : actors) {
        for (auto& object : objects) {
            Collision::CollisionResponse(*actor, *object);
        }
    }
    for (int i = 0; i < objects.size(); i++)
    {
        for (int j = i + 1; j < objects.size(); j++)
        {
            Collision::CollisionResponse(*objects[i], *objects[j]);
        }
    }
    for(auto& objects : objects)
    {
		Collision::CollisionResponse(*mainPlayerActor, *objects);
	}
    for (int i = 0; i < actors.size(); i++)
    {
        for (int j = i + 1; j < actors.size(); j++)
        {
            Collision::CollisionResponse(*actors[i], *actors[j]);
        }
    }
    for (auto& actor : actors)
    {
        actor->draw_Hitbox = globalDrawHitboxes;
    }
}

void Scene0g::Render() const
{
    /// Set the background color then clear the screen
    glClearColor(0.33f, 0.0f, 0.33f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	ImDrawList* drawList = ImGui::GetBackgroundDrawList(); // Get the background draw list to draw behind all ImGui windows
	drawList->AddRectFilled(ImVec2(10, 10), ImVec2(400, 100), IM_COL32(0, 0, 0, 150)); // Draw a semi-transparent rectangle as background for text
	drawList->AddText(MainFont, 22.0f, ImVec2(20, 60), IM_COL32(0, 255, 0, 255), "Hello Im a different font then the rest!"); // Draw text with custom font and size
    drawList->AddText(ImVec2(20, 20), IM_COL32(255, 255, 255, 255), "Hello from ImGui!");
	drawList->AddText(ImVec2(20, 40), IM_COL32(255, 255, 0, 255), "Players active: 1"); // Draw text at position (20, 40)

    if (drawInWireMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

      /** Render Main Player **/
    mainPlayerActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    /** Render Main Controller **/
    mainPlayerController->RenderCrossHairs(camera->GetViewMatrix(), camera->GetProjectionMatrix());

    /* Regular Loop for Rendering Bullets */
    for (auto& actor : actors)
    {
        actor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    for (auto& objects : objects)
    {
        objects->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
	}



    glUseProgram(0);
}
