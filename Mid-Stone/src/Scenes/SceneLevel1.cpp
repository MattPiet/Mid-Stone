#include <glew.h>

#include "Scenes/SceneLevel1.h"

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
}

void SceneLevel1::PlayerShoot()
{
    mainPlayerActor->GetAnimator()->playAnimationClip("Attack");
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
    bullet->draw_Hitbox = true;
    bullet->ConfigureLifeSpan(4.0f);
    bullet->RegisterExpiredCallback([this](const Actor2D& actor)
    {
        auto impact = std::make_unique<Actor2D>();
        impact->OnCreate("sprites/impact.png");
        impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
        impact->ConfigureLifeSpan(1.0f);
        spawnQueue.emplace(std::move(impact));
    });
    bullet->RegisterCollisionCallback([this](Actor2D& actor, const Actor2D& otherActor)
    {
        if (otherActor.Tag() == Actor_tags::target)
        {
            actor.ConfigureLifeSpan(0.01f);
            auto impact = std::make_unique<Actor2D>();
            impact->OnCreate("sprites/impact.png");
            impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
            impact->ConfigureLifeSpan(1.0f);
            spawnQueue.emplace(std::move(impact));
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
    auto mainPlayerClipIdle = new AnimationClip(
        AnimationClip::PlayMode::PINGPONG,
        0.2f,
        2, 17,
        9, 13
    );
    auto mainPlayerClipRun = new AnimationClip(
        AnimationClip::PlayMode::ONCE,
        0.2f,
        2, 17,
        20, 28
    );
    auto mainPlayerClipAttack = new AnimationClip(
        AnimationClip::PlayMode::ONCE,
        0.2f,
        2, 17,
        29, 32
    );
    auto mainPlayerClipHurt = new AnimationClip(
        AnimationClip::PlayMode::ONCE,
        0.2f,
        2, 17,
        1, 3
    );
    // TODO, The clip not being directly related to the spritesheet is weird no?
    mainPlayerActor->GetAnimator()->addDefaultAnimationClip("Idle", mainPlayerClipIdle);
    mainPlayerActor->GetAnimator()->addAnimationClip("Run", mainPlayerClipRun);
    mainPlayerActor->GetAnimator()->addAnimationClip("Attack", mainPlayerClipAttack);
    mainPlayerActor->GetAnimator()->addAnimationClip("Hurt", mainPlayerClipHurt);
    mainPlayerActor->GetAnimator()->playAnimationClip("Idle");

    mainPlayerActor->GetEntity()->SetPosition(Vec3(-80.0f, -40.0f, 0.0f));
    mainPlayerActor->draw_Hitbox = true;

    /** Main Player Controller **/

    mainPlayerController = std::make_unique<PlayerController>();
    mainPlayerController->OnCreate("sprites/crosshairs.png");
    mainPlayerController->SetPossessedActor(mainPlayerActor.get());

    /** Terrain Objects **/

    // Terrain Size 25.6
    const std::vector terrainPositions = {
        std::make_pair(Vec3(-30.0f, 30.0f, 0.0f), 0.0f),
        std::make_pair(Vec3(-4.4f, 30.0f, 0.0f), 0.0f),
        std::make_pair(Vec3(21.2f, 30.0f, 0.0f), 0.0f),
        std::make_pair(Vec3(46.8f, 30.0f, 0.0f), 0.0f),
        std::make_pair(Vec3(40.0f, -20.0f, 0.0f), 0.0f),
        std::make_pair(Vec3(0.0f, -10.0f, 0.0f), 90.0f),
        std::make_pair(Vec3(0.0f, -35.5f, 0.0f), 90.0f),
        std::make_pair(Vec3(60.0f, 0.0f, 0.0f), 90.0f),
        std::make_pair(Vec3(60.0f, 25.6f, 0.0f), 90.0f),

    };

    /** Terrain Setup **/
    for (const auto& position : terrainPositions)
    {
        auto terrain = std::make_unique<Actor2D>();
        terrain->OnCreate("sprites/horizontal-platform.png");
        terrain->GetEntity()->SetPosition(position.first);
        terrain->GetEntity()->SetOrientation(
            terrain->GetEntity()->GetOrientation() * QMath::angleAxisRotation(position.second, Vec3(0.0f, 0.0f, 1.0f)));
        terrain->GetEntity()->SetScale(Vec3(1.0f, 1.0f, 1.0f));
        terrain->ReBuildAll("sprites/horizontal-platform.png");
        terrain->isStatic = true;
        terrain->draw_Hitbox = true;
        terrainActors.emplace_back(std::move(terrain));
    }

    /** Target Setup **/
    target = std::make_unique<Actor2D>();
    if (!target->OnCreate("sprites/guy_tilesheet.png", 2, 17))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
    auto targetPlayerClipIdle = new AnimationClip(
        AnimationClip::PlayMode::PINGPONG,
        0.2f,
        2, 17,
        9, 13
    );
    auto targetPlayerClipRun = new AnimationClip(
        AnimationClip::PlayMode::ONCE,
        0.2f,
        2, 17,
        20, 28
    );
    auto targetPlayerClipAttack = new AnimationClip(
        AnimationClip::PlayMode::ONCE,
        0.2f,
        2, 17,
        29, 32
    );
    auto targetPlayerClipHurt = new AnimationClip(
        AnimationClip::PlayMode::ONCE,
        0.1f,
        2, 17,
        1, 3
    );
    // TODO, The clip not being directly related to the spritesheet is weird no?
    target->GetAnimator()->addDefaultAnimationClip("Idle", targetPlayerClipIdle);
    target->GetAnimator()->addAnimationClip("Run", targetPlayerClipRun);
    target->GetAnimator()->addAnimationClip("Attack", targetPlayerClipAttack);
    target->GetAnimator()->addAnimationClip("Hurt", targetPlayerClipHurt);
    target->GetAnimator()->playAnimationClip("Idle");
    target->SetTag(Actor_tags::target);
    target->RegisterCollisionCallback([this](const Actor2D& actor, const Actor2D& otherActor)
    {
        actor.GetAnimator()->playAnimationClip("Hurt");
    });
    target->isStatic = true;
    target->GetEntity()->SetPosition(Vec3(40.0f, -10.0f, 0.0f));
    target->draw_Hitbox = true;


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

    /** Update Main Player and Target **/
    mainPlayerActor->Update(deltaTime);
    target->Update(deltaTime);

    /** Update Actors **/
    for (auto& actor : actors)
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
        Collision::CollisionResponse(*actor, *target);
    }


    /** Spawn Queue **/
    while (!spawnQueue.empty())
    {
        actors.emplace_back(std::move(spawnQueue.front()));
        spawnQueue.pop();
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


void SceneLevel1::RenderGUI()
{
}


void SceneLevel1::Render() const
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
    target->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());

    glUseProgram(0);
}
