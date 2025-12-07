#include <glew.h>

#include "Scenes/SceneLevel1.h"

#include <map>
#include <queue>

#include "Graphics/CameraController.h"
#include "Utils/Debug.h"
#include <Utils/MemoryMonitor.h>

#include "Physics/Collisions.h"


SceneLevel1::SceneLevel1()
{
    Debug::Info("Created Scene Level 1: ", __FILE__, __LINE__);
}

SceneLevel1::~SceneLevel1()
{
    Debug::Info("Deleted Scene Level 1: ", __FILE__, __LINE__);
}

void SceneLevel1::OnDestroy()
{
    Debug::Info("OnDestroy Scene Level 1: ", __FILE__, __LINE__);

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

void SceneLevel1::PlayerShoot()
{
    mainPlayerActor->GetAnimator()->PlayAnimationClip("Attack");
    const Vec3 currentCrossHairsPosition = mainPlayerController->GetCrossHairsPosition();
    auto bullet = std::make_unique<Actor2D>();
    bullet->OnCreate("sprites/punch.png");
    bullet->GetEntity()->SetPosition(currentCrossHairsPosition);
    const auto crossHairsQuaternion = mainPlayerController->GetCrossHairsRotation();
    const auto forward = Vec3(1.0f, 0.0f, 0.0f);
    const Vec3 rotatedVector = crossHairsQuaternion * forward * QMath::inverse(crossHairsQuaternion); // QPQ-1 Formula
    const Vec3 finalVelocity = rotatedVector * 100.0f;
    // TODO We have to rebuild after adjusting Scale, change this
    bullet->GetEntity()->SetScale(Vec3(0.3f, 0.3f, 0.3f));
    bullet->ReBuildAll("sprites/punch.png");
    bullet->GetEntity()->AdjustOrientation(crossHairsQuaternion);
    bullet->GetEntity()->SetVelocity(finalVelocity);
    // bullet->draw_Hitbox = true;
    bullet->ConfigureLifeSpan(4.0f);
    bullet->SetTag(Actor_tags::bullets);
    bullet->RegisterExpiredCallback([this](const Actor2D& actor)
    {
        auto impact = std::make_unique<Actor2D>();
        impact->OnCreate("sprites/impact.png", 2, 4);
        impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
        impact->ConfigureLifeSpan(0.27f);
        const auto clip = new AnimationClip(AnimationClip::Play_mode::once, 0.03f, 2, 4, 0, 7);
        impact->GetAnimator()->AddDefaultAnimationClip("Idle", clip);
        impact->GetAnimator()->PlayAnimationClip("Idle");
        spawnQueue.emplace(std::move(impact));
    });
    bullet->RegisterCollisionCallback([this](Actor2D& actor, const Actor2D& otherActor)
    {
        if (otherActor.Tag() == Actor_tags::target)
        {
            actor.ConfigureLifeSpan(0.01f);
        }
    });
    spawnQueue.emplace(std::move(bullet));
}

bool SceneLevel1::OnCreate()
{
    Debug::Info("On Create Scene Level 1: ", __FILE__, __LINE__);

    /** Main Player **/
    /** Main Actor **/
    mainPlayerActor = std::make_unique<Actor2D>();
    if (!mainPlayerActor->OnCreate("sprites/guy_tilesheet.png", 2, 17))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
    mainPlayerActor->GetGuns()->SetGunType(Guns::Gun_type::shotgun);
    // mainPlayerActor->GetGuns()->setMixer(mixer);
    // mainPlayerActor->GetGuns()->funnyNoises = true;
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

    mainPlayerActor->GetEntity()->SetPosition(Vec3(-80.0f, -40.0f, 0.0f));
    // mainPlayerActor->draw_Hitbox = true;

    /** Main Player Controller **/

    mainPlayerController = std::make_unique<PlayerController>();
    mainPlayerController->OnCreate("sprites/crosshairs.png");
    mainPlayerController->SetPossessedActor(mainPlayerActor.get());

    /** Terrain Objects **/

    std::map<int, std::string> terrainFilenameMap = {
        {20, "sprites/walls/rock_wall_1_x_20.png"},
        {10, "sprites/walls/rock_wall_1_x_10.png"},
        {5, "sprites/walls/rock_wall_1_x_5.png"},
        {4, "sprites/walls/rock_wall_1_x_4.png"},
        {3, "sprites/walls/rock_wall_1_x_3.png"}
    };

    const std::vector<std::tuple<Vec3, float, int>> terrainData = {
        {Vec3(-30.0f, 50.0f, 0.0f), 0.0f, 20}, // Top Wall
        {Vec3(66.0f, 50.0f, 0.0f), 0.0f, 10}, // Top Wall
        {Vec3(-30.0f, -50.0f, 0.0f), 0.0f, 20}, // Bottom Wall
        {Vec3(66.0f, -50.0f, 0.0f), 0.0f, 10}, // Bottom Wall
        {Vec3(-97.2f, 0.0f, 0.0f), 90.0f, 20}, // Left Wall
        {Vec3(101.2f, 0.0f, 0.0f), 90.0f, 20}, // Right Wall
        {Vec3(-62.0f, -3.2f, 0.0f), 0.0f, 10}, // Left Platform
        {Vec3(66.0f, -3.2f, 0.0f), 0.0f, 10}, // Right Platform
        {Vec3(5.2f, -43.6f, 0.0f), 0.0f, 5}, // Middle PLatform
        {Vec3(62.8f, -43.6f, 0.0f), 0.0f, 5}, // Bottom Right Platform
    };

    /** Terrain Setup **/
    for (const auto& data : terrainData)
    {
        auto terrain = std::make_unique<Actor2D>();
        std::string terrainFilename = terrainFilenameMap[std::get<2>(data)];
        terrain->OnCreate(terrainFilename.c_str());
        terrain->GetEntity()->SetPosition(std::get<0>(data));
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() *
            QMath::angleAxisRotation(std::get<1>(data), Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.0f, 1.0f, 1.0f));
        terrain->ReBuildAll(terrainFilename.c_str());
        terrain->isStatic = true;
        // terrain->draw_Hitbox = true;
        terrainActors.emplace_back(std::move(terrain));
    }


    /** Target Setup **/

    const std::vector<Vec3> targetData = {
        {Vec3(-70.0f, 3.2f, 0.0f)},
        {Vec3(70.2f, 3.2f, 0.0f)},
        {Vec3(67.0f, -37.2f, 0.0f)},
        {Vec3(0.0f, -37.2f, 0.0f)},
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
    MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/CrabRave.wav", true);
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


void SceneLevel1::Update(const float deltaTime)
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

    /** Target Cleanup **/
    targets.erase(
        std::remove_if(targets.begin(), targets.end(),
                       [](const std::unique_ptr<Actor2D>& e) { return e->HasExpired(); }),
        targets.end()
    );

    /** Update Main Player **/
    mainPlayerActor->Update(deltaTime);

    for (auto& impact : impacts) spawnQueue.emplace(std::move(impact));
    if (impacts.size() > 0)
    {
        impacts.clear();
        canShoot = true;
    }

    /** Update Targets **/
    for (const auto& targetActor : targets)
    {
        targetActor->Update(deltaTime);
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

void SceneLevel1::HandleEvents(const SDL_Event& sdlEvent)
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
                    /** Load and play music **/
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


void SceneLevel1::RenderGUI()
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
            RequestChangeScene(Scene_number::scene0_g);
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


void SceneLevel1::Render() const
{
    /** Render Setup **/
    glClearColor(0.241f, 0.265f, 0.422f, 1.0f);
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
    /* Regular Loop for Rendering Players */
    for (auto& targetActor : targets)
    {
        targetActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    glUseProgram(0);
}
