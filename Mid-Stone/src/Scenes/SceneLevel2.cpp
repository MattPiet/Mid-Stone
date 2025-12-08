#include <glew.h>

#include "Scenes/SceneLevel2.h"

#include <queue>

#include "Graphics/CameraController.h"
#include "Utils/Debug.h"
#include <Utils/MemoryMonitor.h>

#include "Physics/Collisions.h"


SceneLevel2::SceneLevel2()
{
    Debug::Info("Created Scene Level 2: ", __FILE__, __LINE__);
}

SceneLevel2::~SceneLevel2()
{
    Debug::Info("Deleted Scene Level 2: ", __FILE__, __LINE__);
}

void SceneLevel2::OnDestroy()
{
    Debug::Info("OnDestroy Scene Level 2: ", __FILE__, __LINE__);

    /** Delete Main Player **/
    mainPlayerActor.reset();
    mainPlayerController.reset();

    /** Camera **/
    cameraController.reset();
    camera.reset();
    //// Turn off audio
    if (mixer)
    {
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }
}



bool SceneLevel2::OnCreate()
{
    Debug::Info("On Create Scene Level 2: ", __FILE__, __LINE__);

    canShoot = true;

    /** Main Player **/
    /** Main Actor **/
    mainPlayerActor = std::make_unique<Actor2D>();
    if (!mainPlayerActor->OnCreate("sprites/guy_tilesheet.png", 2, 17))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
    mainPlayerActor->GetGuns()->SetGunType(Guns::Gun_type::pistol);
    mainPlayerActor->GetGuns()->setBulletSprite("sprites/punch.png");
    mainPlayerActor->GetGuns()->setPistolExpiration(5.5f);
    mainPlayerActor->GetGuns()->setSpriteScale(0.3);
    const auto mainPlayerClipIdle = new AnimationClip(
        AnimationClip::Play_mode::pingpong,
        0.2f,
        2, 17,
        9, 13
    );
    const auto mainPlayerClipRun = new AnimationClip(
        AnimationClip::Play_mode::once,
        0.2f,
        2, 17,
        20, 28
    );
    const auto mainPlayerClipAttack = new AnimationClip(
        AnimationClip::Play_mode::once,
        0.2f,
        2, 17,
        29, 32
    );
    const auto mainPlayerClipHurt = new AnimationClip(
        AnimationClip::Play_mode::once,
        0.2f,
        2, 17,
        1, 3
    );
    // TODO, The clip not being directly related to the spritesheet is weird no?
    mainPlayerActor->GetAnimator()->AddDefaultAnimationClip("Idle", mainPlayerClipIdle);
    mainPlayerActor->GetAnimator()->AddAnimationClip("Run", mainPlayerClipRun);
    mainPlayerActor->GetAnimator()->AddAnimationClip("Attack", mainPlayerClipAttack);
    mainPlayerActor->GetAnimator()->AddAnimationClip("Hurt", mainPlayerClipHurt);
    mainPlayerActor->GetAnimator()->PlayAnimationClip("Idle");

    mainPlayerActor->GetEntity()->SetPosition(Vec3(-90.0f, -40.0f, 0.0f));
    // mainPlayerActor->draw_Hitbox = true;

    /** Main Player Controller **/

    mainPlayerController = std::make_unique<PlayerController>();
    mainPlayerController->OnCreate("sprites/crosshairs.png");
    mainPlayerController->SetPossessedActor(mainPlayerActor.get());
    mainPlayerController->MoveAim(45.f);

    /** Terrain Objects **/
	//Positions for 1x1 terrain pieces
    const std::vector terrain1Positions = {
        std::make_pair(Vec3(-48.f, -50.6f, 0.f), 0.f),
		std::make_pair(Vec3(-22.4f, 26.f, 0.f), 0.f),
        std::make_pair(Vec3(3.2f, -50.6f, 0.f), 0.f),
        std::make_pair(Vec3(28.8f, 26.f, 0.f), 0.f),
        std::make_pair(Vec3(54.4f, -50.6f, 0.f), 0.f),
        std::make_pair(Vec3(80.f, 26.f, 0.f), 0.f),
    };
	//Positions for 1x5 terrain pieces
    const std::vector terrain5Positions = {
        std::make_pair(Vec3(-96.f, 41.6f, 0.f), -90.f),
        std::make_pair(Vec3(-96.f, -41.6f, 0.f), -90.f),
        std::make_pair(Vec3(96.f, 41.6f, 0.f), -90.f),
        std::make_pair(Vec3(96.f, -41.6f, 0.f), -90.f),

        std::make_pair(Vec3(-86.4f, 44.4f, 0.f), 45.f),
        std::make_pair(Vec3(-60.8f, 44.4f, 0.f), -45.f),
        std::make_pair(Vec3(-60.8f, -44.4f, 0.f), -45.f),
        std::make_pair(Vec3(-35.2f, -44.4f, 0.f), 45.f),
        std::make_pair(Vec3(-35.2f, 44.4f, 0.f), 45.f),
		std::make_pair(Vec3(-9.6f, 44.4f, 0.f), -45.f),
        std::make_pair(Vec3(-9.6f, -44.4f, 0.f), -45.f),
        std::make_pair(Vec3(16.f, -44.4f, 0.f), 45.f),
        std::make_pair(Vec3(16.f,  44.4f, 0.f), 45.f),
        std::make_pair(Vec3(41.6, 44.4f , 0.f), -45.f),
        std::make_pair(Vec3(41.6, -44.4f, 0.f), -45.f),
        std::make_pair(Vec3(67.2, -44.4f, 0.f), 45.f),
        std::make_pair(Vec3(67.2,  44.4f, 0.f), 45.f),
        
    };
	//Positions for 1x10 terrain pieces
    const std::vector terrain10Positions = {
		std::make_pair(Vec3(-96.f, 0.f, 0.f), 90.f),
        std::make_pair(Vec3(96.f, 0.f, 0.f), 90.f),
        std::make_pair(Vec3(-73.6f, -22.f, 0.f), 90.f),
        std::make_pair(Vec3(-48.f, 22.f, 0.f), 90.f),
        std::make_pair(Vec3(-22.4f, -22.f, 0.f), 90.f),
        std::make_pair(Vec3(3.2f, 22.f, 0.f), 90.f),
        std::make_pair(Vec3(28.8f, -22.f, 0.f), 90.f),
        std::make_pair(Vec3(54.4f, 22.f, 0.f), 90.f),
        std::make_pair(Vec3(80.f, -22.f, 0.f), 90.f),
    };
	// Positions for 1x20 terrain pieces
    const std::vector terrain20Positions = {
        std::make_pair(Vec3(-32.f, 54.f, 0.f), 0.f),
        std::make_pair(Vec3(32.f, 54.f, 0.f), 0.f),
        std::make_pair(Vec3(-32.f, -54.f, 0.f), 0.f),
        std::make_pair(Vec3(32.f, -54.f, 0.f), 0.f),
    };

    /** Terrain Setup **/
    //Terrain Setup for 1x1 pieces
    for (const auto& position : terrain1Positions)
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/walls/rock_wall_1_x_1.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.f, 1.f, 1.f));
        terrain->ReBuildAll("sprites/walls/rock_wall_1_x_1.png");
        terrain->isStatic = true;
        terrainActors.emplace_back(std::move(terrain));
    }
    //Terrain Setup for 1x5 pieces
    for (const auto& position : terrain5Positions) 
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/walls/rock_wall_1_x_5.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.f, 1.f, 1.f));
        terrain->ReBuildAll("sprites/walls/rock_wall_1_x_5.png");
        terrain->isStatic = true;
        terrainActors.emplace_back(std::move(terrain));
    }
	//Terrain Setup for 1x10 pieces
    for (const auto& position : terrain10Positions)
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/walls/rock_wall_1_x_10.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
			terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.f, 1.f, 1.f));
        terrain->ReBuildAll("sprites/walls/rock_wall_1_x_10.png");
        terrain->isStatic = true;
        terrainActors.emplace_back(std::move(terrain));
    }

    //Terrain Setup for 1x20 pieces
    for (const auto& position : terrain20Positions)
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/walls/rock_wall_1_x_20.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.0f, 1.0f, 1.0f));
        terrain->ReBuildAll("sprites/walls/rock_wall_1_x_20.png");
        terrain->isStatic = true;
         //terrain->draw_Hitbox = true;
        terrainActors.emplace_back(std::move(terrain));
    }

    /** Target Setup **/
	const std::vector<Vec3> targetData = {
        {Vec3(-48.f, -44.0f, 0.0f)},
        {Vec3(-22.4f, 32.6f, 0.f)},
        {Vec3(3.2f, -44.0f, 0.f)},
        {Vec3(28.8f, 32.6f, 0.f)},
        {Vec3(54.4f, -44.0f, 0.f)},
        {Vec3(80.0f, 32.6f, 0.f)},
    };
    
    
    for (const auto& data : targetData)
    {
        auto target = std::make_unique<Actor2D>();
        if (!target->OnCreate("sprites/guy_tilesheet.png", 2, 17))
        {
            std::cout << "Failed to create test actor spritesheet\n";
            return false;
        }
        const auto targetPlayerClipIdle = new AnimationClip(
            AnimationClip::Play_mode::pingpong,
            0.2f,
            2, 17,
            9, 13
        );
        const auto targetPlayerClipRun = new AnimationClip(
            AnimationClip::Play_mode::once,
            0.2f,
            2, 17,
            20, 28
        );
        const auto targetPlayerClipAttack = new AnimationClip(
            AnimationClip::Play_mode::once,
            0.2f,
            2, 17,
            28, 31
        );
        const auto targetPlayerClipHurt = new AnimationClip(
            AnimationClip::Play_mode::once,
            0.1f,
            2, 17,
            0, 2
        );
        const auto targetPlayerClipDeath = new AnimationClip(
            AnimationClip::Play_mode::once,
            0.1f,
            2, 17,
            3, 7
        );
        // TODO, The clip not being directly related to the spritesheet is weird no?
        target->GetAnimator()->AddDefaultAnimationClip("Idle", targetPlayerClipIdle);
        target->GetAnimator()->AddAnimationClip("Run", targetPlayerClipRun);
        target->GetAnimator()->AddAnimationClip("Attack", targetPlayerClipAttack);
        target->GetAnimator()->AddAnimationClip("Hurt", targetPlayerClipHurt);
        target->GetAnimator()->AddAnimationClip("Death", targetPlayerClipDeath);
        target->GetAnimator()->PlayAnimationClip("Idle");
        target->SetHealth(1);
        target->SetTag(Actor_tags::target);
        target->RegisterCollisionCallback([this](Actor2D& actor, const Actor2D& otherActor)
            {
                if (otherActor.Tag() == Actor_tags::bullets)
                {
                    actor.GetAnimator()->PlayAnimationClip("Hurt");
                    actor.TakeDamage(1);
                }
            });
        target->isStatic = true;
        target->GetEntity()->SetPosition(data);
        targets.emplace_back(std::move(target));
        // target->draw_Hitbox = true;
    }

    /** Audio **/
     /** Load and play music **/
        /** Initiate Libraries **/
    SDL_Init(SDL_INIT_AUDIO);
    MIX_Init();
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer)
    {
        std::cout << "Failed to create mixer: %s\n", SDL_GetError();
        return false;
    }
    PlayAudio = SceneManager::GetAudioStateStatic();
    master_volume = SceneManager::GetMasterVolumeStatic();
    MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/level2Music.mp3", true);
    MIX_SetMasterGain(mixer, master_volume);
    MIX_PlayAudio(mixer, Music);
    MIX_DestroyAudio(Music);
    mainPlayerActor->GetGuns()->SetMixer(mixer);
    mainPlayerActor->GetGuns()->funnyNoises = true;

    /** Camera **/
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());
    return true;
}


void SceneLevel2::Update(const float deltaTime)
{
    /** Main Player Controller **/
    if (leftPressed && !rightPressed)
    {
        mainPlayerController->MoveAim(1.0f);
    }
    if (rightPressed && !leftPressed)
    {
        mainPlayerController->MoveAim(-1.0f);
    }

    /* Actor Cleanup */
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
                       [](const std::unique_ptr<Actor2D>& e) { return e->HasExpired(); }),
        actors.end()
    );
    /* Target Cleanup */
    targets.erase(
        std::remove_if(targets.begin(), targets.end(),
            [](const std::unique_ptr<Actor2D>& e) { return e->HasExpired(); }),
        targets.end()
    );


    /** Update Main Player and Target **/
    mainPlayerActor->Update(deltaTime);
    
    /* Update Targets */
    for (const auto& target : targets)
    {
        target->Update(deltaTime);
	}

    for (auto& impact : impacts) spawnQueue.emplace(std::move(impact));
    if (impacts.size() > 0)
    {
        impacts.clear();
        canShoot = true;
    }

    /** Spawn Queue **/
    while (!spawnQueue.empty())
    {
        actors.emplace_back(std::move(spawnQueue.front()));
        spawnQueue.pop();
    }

    /** Check Targets Destroyed **/
    if (targets.empty())
    {
        levelFinished = true;
    }

    /** Update Actors **/
    for (const auto& actor : actors)
    {

        //actor->Update(deltaTime);
        // --- 1. compute intended movement ---
        Vec3 oldPos = actor->GetEntity()->GetPosition();
        actor->Update(deltaTime);  // updates velocity
        Vec3 newPos = actor->GetEntity()->GetPosition();

        Vec3 movement = newPos - oldPos;
        float dist = VMath::mag(movement);

        const float maxStep = 1.0f;
        int steps = (dist > 0.0f) ? ceil(dist / maxStep) : 1;

        Vec3 stepVec = movement / (float)steps;

        // Reset to old position
        Vec3 pos = oldPos;

        for (int i = 0; i < steps; i++)
        {
            // move attempt
            Vec3 attempt = pos + stepVec;
            actor->GetEntity()->SetPosition(attempt);

            // collisions
            for (const auto& terrain : terrainActors)
                Collision::CollisionResponse(*actor, *terrain);

            for (const auto& target : targets)
                Collision::CollisionResponse(*actor, *target);

            // IMPORTANT: update pos to whatever collision resolved
            pos = actor->GetEntity()->GetPosition();
        }
        actor->FaceVelocity(deltaTime);
    }

}

void SceneLevel2::HandleEvents(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.type)
    {
    case SDL_EVENT_KEY_UP:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_A:
            leftPressed = false;
            break;
        case SDL_SCANCODE_D:
            rightPressed = false;
            break;
        default: ;
        }
        break;
    case SDL_EVENT_KEY_DOWN:
        switch (sdlEvent.key.scancode)
        {
        case SDL_SCANCODE_A:
            leftPressed = true;
            break;
        case SDL_SCANCODE_D:
            rightPressed = true;
            break;
        case SDL_SCANCODE_SPACE:
            {
            if (canShoot)
            {
                mainPlayerActor->GetAnimator()->PlayAnimationClip("Attack");
                //PlayerShoot();
                auto bullets = mainPlayerActor->GetGuns()->Shoot(mainPlayerController.get(), impacts);
                for (auto& bullet : bullets) spawnQueue.emplace(std::move(bullet));
                canShoot = false;
                
            }
            break;
            }

        default: ;
        }
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
        default: ;
        }
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        switch (sdlEvent.button.button)
        {
        case SDL_BUTTON_MIDDLE:
            cameraController->OnMouseButtonReleased(2);
            break;
        default: ;
        }
        break;
    default:
        break;
    }
}


void SceneLevel2::RenderGUI()
{
    /** Level Finish Popup **/
    if (levelFinished)
        ImGui::OpenPopup("Level Finished!");

    if (ImGui::BeginPopupModal("Level Finished!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Congratulations, all targets have been hit");
        ImGui::Separator();

        if (ImGui::Button("Go to next level", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            RequestChangeScene(Scene_number::scene_level_3);
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Restart Level", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            RequestRestartScene();
        }
        ImGui::SameLine();
        if (ImGui::Button("Quit Game", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            RequestQuitApplication();
        }
        ImGui::EndPopup();
    }


    ImVec4 r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 g = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    ImVec4 b = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);

    UIManager::StartInvisibleWindow("Hidden Window1", ImVec2(0, 0)); // we start an invisible window here

    UIManager::PushButtonStyle(b, g, r, 90.0f); // pushing button style

    if (ImGui::Button("Mute Audio")) { // making a button to toggle hitbox drawing
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

    UIManager::EndWindow(); // end the invisible window

    // Invisible window for gun selection
    UIManager::StartInvisibleWindow("GunSelector", ImVec2(0, 1000));
    UIManager::PushButtonStyle(b, g, r, 5.0f);

    if (ImGui::Button("Pistol")) {
        mainPlayerActor->GetGuns()->SetGunType(Guns::Gun_type::pistol);
    }

    if (ImGui::Button("Shotgun")) {
        mainPlayerActor->GetGuns()->SetGunType(Guns::Gun_type::shotgun);
    }

    if (ImGui::Button("Rifle")) {
        mainPlayerActor->GetGuns()->SetGunType(Guns::Gun_type::rifle);
    }

    UIManager::PopButtonStyle();
    UIManager::EndWindow();
}


void SceneLevel2::Render() const
{
    /** Render Setup **/
    glClearColor(0.541f, 0.765f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /** Render Main Player **/
    mainPlayerActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    /** Render Main Controller **/
    mainPlayerController->RenderCrossHairs(camera->GetViewMatrix(), camera->GetProjectionMatrix());

    /** Render all Actors **/
    /** Render Terrain **/
    for (const auto& terrainActor : terrainActors)
    {
        terrainActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }
    /* Regular Loop for Rendering Players */
    for (auto& actor : actors)
    {
        actor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }
    /** Render Target **/
   /* Regular Loop for Rendering Players */
    for (auto& targetActor : targets)
    {
        targetActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }


    glUseProgram(0);
}
