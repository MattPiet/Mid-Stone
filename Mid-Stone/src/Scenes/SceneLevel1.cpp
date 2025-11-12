#include <glew.h>

#include "Scenes/SceneLevel1.h"

#include <queue>

#include "Graphics/CameraController.h"
#include "Utils/Debug.h"


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
    const Vec3 currentCrossHairsPosition = mainPlayerController->GetCrossHairsPosition();
    auto bullet = std::make_unique<Actor2D>();
    bullet->OnCreate("sprites/fist.png");
    bullet->GetEntity()->SetPosition(currentCrossHairsPosition);
    const auto crossHairsQuaternion = mainPlayerController->GetCrossHairsRotation();
    const auto forward = Vec3(1.0f, 0.0f, 0.0f);
    const Vec3 rotatedVector = crossHairsQuaternion * forward * QMath::inverse(crossHairsQuaternion); // QPQ-1 Formula
    const Vec3 finalVelocity = rotatedVector * 100.0f;
    bullet->GetEntity()->AdjustOrientation(crossHairsQuaternion);
    bullet->GetEntity()->SetVelocity(finalVelocity);
    bullet->ConfigureLifeSpan(1.0f);
    bullet->RegisterExpiredCallback([this](const Actor2D& actor)
    {
        auto impact = std::make_unique<Actor2D>();
        impact->OnCreate("sprites/impact.png");
        impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
        impact->ConfigureLifeSpan(1.0f);
        spawnQueue.emplace(std::move(impact));
    });
    spawnQueue.emplace(std::move(bullet));
}


void SceneLevel1::Update(const float deltaTime)
{
    /** Main Player Controller **/
    if (leftPressed && !rightPressed)
    {
        mainPlayerController->MoveAim(2.0f);
    }
    if (rightPressed && !leftPressed)
    {
        mainPlayerController->MoveAim(-2.0f);
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

bool SceneLevel1::OnCreate()
{
    Debug::Info("On Create Scene Level 1: ", __FILE__, __LINE__);
    /** Main Player **/
    /** Main Actor **/
    mainPlayerActor = std::make_unique<Actor2D>();
    if (!mainPlayerActor->OnCreate("sprites/Idle.png", 1, 3))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
    // TODO Why do i set rows and columns twice my n word
    const auto mainPlayerClipIdle = new AnimationClip(
        AnimationClip::PlayMode::PINGPONG,
        0.1f,
        1, 3
    );
    // TODO, The clip not being directly related to the spritesheet is weird no?
    mainPlayerActor->GetAnimator()->addAnimationClip("Idle", mainPlayerClipIdle);
    mainPlayerActor->GetAnimator()->playAnimationClip("Idle");
    mainPlayerActor->draw_Hitbox = true;

    /** Main Player Controller **/

    mainPlayerController = std::make_unique<PlayerController>();
    mainPlayerController->OnCreate("sprites/crosshairs.png");
    mainPlayerController->SetPossessedActor(mainPlayerActor.get());

    /** Camera **/
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());
    return true;
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
    /* Regular Loop for Rendering Players */
    for (auto& actor : actors)
    {
        actor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    glUseProgram(0);
}
