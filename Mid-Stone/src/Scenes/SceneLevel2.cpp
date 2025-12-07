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
}

void SceneLevel2::PlayerShoot()
{
    mainPlayerActor->GetAnimator()->PlayAnimationClip("Attack");
    const Vec3 currentCrossHairsPosition = mainPlayerController->GetCrossHairsPosition();
    auto bullet = std::make_unique<Actor2D>();
    bullet->OnCreate("sprites/punch.png");
    bullet->GetEntity()->SetPosition(currentCrossHairsPosition);
    const auto crossHairsQuaternion = mainPlayerController->GetCrossHairsRotation();
    const auto forward = Vec3(1.0f, 0.0f, 0.0f);
    const Vec3 rotatedVector = crossHairsQuaternion * forward * QMath::inverse(crossHairsQuaternion); // QPQ-1 Formula
    const Vec3 finalVelocity = rotatedVector * 150.0f;\
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

bool SceneLevel2::OnCreate()
{
    Debug::Info("On Create Scene Level 2: ", __FILE__, __LINE__);

    /** Main Player **/
    /** Main Actor **/
    mainPlayerActor = std::make_unique<Actor2D>();
    if (!mainPlayerActor->OnCreate("sprites/guy_tilesheet.png", 2, 17))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
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

    /** Terrain Objects **/
	//Positions for 1x1 terrain pieces
    const std::vector terrain1Positions = {
        std::make_pair(Vec3(-48.0f, -50.6f, 0.f), 0.f),
		std::make_pair(Vec3(-22.6f, 26.f, 0.f), 0.f),
    };
	//Positions for 1x5 terrain pieces
    const std::vector terrain3Positions = {
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
        
    };
	//Positions for 1x10 terrain pieces
    const std::vector terrain10Positions = {
		std::make_pair(Vec3(-96.f, 0.f, 0.f), 90.f),
        std::make_pair(Vec3(96.f, 0.f, 0.f), 90.f),
        std::make_pair(Vec3(-73.4f, -22.f, 0.f), 90.f),
        std::make_pair(Vec3(-48.0f, 22.f, 0.f), 90.f),
        std::make_pair(Vec3(-22.6f, -22.f, 0.f), 90.f),
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
        terrain->ReBuildAll("sprites/rock_wall_1_x_1.png");
        terrain->isStatic = true;
        terrainActors.emplace_back(std::move(terrain));
    }
    //Terrain Setup for 1x5 pieces
    for (const auto& position : terrain3Positions) 
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/rock_wall_1_x_5.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.f, 1.f, 1.f));
        terrain->ReBuildAll("sprites/rock_wall_1_x_5.png");
        terrain->isStatic = true;
        terrainActors.emplace_back(std::move(terrain));
    }
	//Terrain Setup for 1x10 pieces
    for (const auto& position : terrain10Positions)
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/rock_wall_1_x_10.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
			terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.f, 1.f, 1.f));
        terrain->ReBuildAll("sprites/rock_wall_1_x_10.png");
        terrain->isStatic = true;
        terrainActors.emplace_back(std::move(terrain));
    }

    //Terrain Setup for 1x20 pieces
    for (const auto& position : terrain20Positions)
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/rock_wall_1_x_20.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.0f, 1.0f, 1.0f));
        terrain->ReBuildAll("sprites/rock_wall_1_x_20.png");
        terrain->isStatic = true;
         //terrain->draw_Hitbox = true;
        terrainActors.emplace_back(std::move(terrain));
    }

    /** Target Setup **/
	const std::vector<Vec3> targetData = {
        {Vec3(-48.0f, -44.0f, 0.0f)},
        {Vec3(-22.6f, 32.6f, 0.f)},
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

    /** Update Actors **/
    for (const auto& actor : actors)
    {
        actor->Update(deltaTime);
        actor->FaceVelocity(deltaTime);
    }

    for (const auto& actor : actors)
    {
        for (const auto& terrainActor : terrainActors)
        {
            Collision::CollisionResponse(*actor, *terrainActor);
        }
        for (const auto& target : targets)
        {
            Collision::CollisionResponse(*actor, *target);
		}
    }


    /** Spawn Queue **/
    while (!spawnQueue.empty())
    {
        actors.emplace_back(std::move(spawnQueue.front()));
        spawnQueue.pop();
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
                PlayerShoot();
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
