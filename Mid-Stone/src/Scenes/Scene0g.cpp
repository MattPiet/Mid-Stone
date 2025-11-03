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

Scene0g::Scene0g() : shader{nullptr},
                     drawInWireMode{false}, master_volume{1.0f}, mixer{nullptr}, sprite_Mesh{nullptr},
                     sprite_Renderer{nullptr}
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
    MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/CrabRaave.wav", true);
    MIX_SetMasterGain(mixer, master_volume);
    MIX_PlayAudio(mixer, Music);
    MIX_DestroyAudio(Music);

    /** Camera **/
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());

    /** Set up Player **/
    
    mainPlayerActor = new Actor2D();
    if (!mainPlayerActor->OnCreate("sprites/Idle.png", 1, 3))
    {
        std::cout << "Failed to create test actor spritesheet\n";
        return false;
    }
    // TODO Why do i set rows and columns twice my n word
    auto mainPlayerClipIdle = new AnimationClip(
        AnimationClip::PlayMode::PINGPONG,
        0.1f,
        1, 3
    );
    // TODO, The clip not being directly related to the spritesheet is weird no?
    mainPlayerActor->getAnimator()->addAnimationClip("Idle", mainPlayerClipIdle);
    mainPlayerActor->getAnimator()->playAnimationClip("Idle");
    mainPlayerActor->draw_Hitbox = true;

    
    /** ---------- **/


    /** Renderer Setup **/
    crossHairsRenderer = new SpriteRenderer();
    crossHairsRenderer->loadImage("sprites/crosshairs.png");
    playerRenderer = new SpriteRenderer();
    playerRenderer->loadImage("sprites/Attack_Top.png", 1, 3);
    bulletsRenderer = new SpriteRenderer();
    bulletsRenderer->loadImage("sprites/fist.png", 2, 8);
    impactRenderer = new SpriteRenderer();
    impactRenderer->loadImage("sprites/impact.png", 2, 4);
    players.emplace_back(std::make_unique<Player>(Vec3{-50, -20, 0}, Vec3{2.0f, 2.0f, 2.0f}));

    for (auto& player : players)
    {
        player->OnCreate(playerRenderer);
    }


    /** Sprite Setup **/
    sprite_Mesh = new SpriteMesh();
    sprite_Mesh->OnCreate();

    shader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
    if (!shader->OnCreate())
    {
        std::cout << "Shader failed ... we have a problem\n";
    }


    return true;
}

void Scene0g::OnDestroy()
{
    Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
    /** Delete Main Player **/
    if (mainPlayerActor != nullptr)
    {   
        delete mainPlayerActor;
        mainPlayerActor = nullptr;
    }


    if (shader != nullptr)
    {
        shader->OnDestroy();
        delete shader;
        shader = nullptr;
    }

    if (sprite_Mesh != nullptr)
    {
        sprite_Mesh->OnDestroy();
        delete sprite_Mesh;
        sprite_Mesh = nullptr;
    }

   


    delete sprite_Renderer;
    sprite_Renderer = nullptr;

    delete playerRenderer;
    playerRenderer = nullptr;

    delete bulletsRenderer;
    bulletsRenderer = nullptr;

    delete crossHairsRenderer;
    crossHairsRenderer = nullptr;

    delete impactRenderer;
    impactRenderer = nullptr;


    cameraController.reset();
    camera.reset();

    //// Turn off audio
    if (mixer)
    {
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }
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
            if (auto bullet = players.front()->Shoot())
            {
                bullet->OnCreate(bulletsRenderer);
                bullet->AdjustHitboxSize(Vec3{-10.f, -10.f, 0.0f});
                // implicit upcast unique_ptr<Bullet> -> unique_ptr<Entity> (default deleter)
                bullet->SetExpiredCallback([this](Entity& e)
                {
                    auto impact = std::make_unique<Entity>(e.GetPosition(), Vec3{1.0f, 1.0f, 1.0f}, Hit_box_type::quad);
                    impact->OnCreate(impactRenderer);
                    impact->SetLifeSpan(1.0f);
                    effects.emplace_back(std::move(impact));
                });
                bullets.emplace_back(std::move(bullet));
            }
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
    ImGui::Begin("Scene 0g Controls");

    ImGui::Text("This is the Scene0g debug window");
    ImGui::Checkbox("Wireframe Mode", &drawInWireMode);

    if (mixer)
    {
        if (ImGui::SliderFloat("Audio Volume", &master_volume, 0.0f, 1.0f))
        {
            MIX_SetMasterGain(mixer, master_volume);
        }
    }
    else
    {
        std::cout << "Audio is Null" << std::endl;
    }

    ImGui::End();
}

void Scene0g::Update(const float deltaTime)
{
    /** Update Main Player **/
    mainPlayerActor->Update(deltaTime);


    /** Update Players **/
    if (pressingLeft && !pressingRight)
    {
        players.front()->MoveAim(2.0f);
    }
    if (pressingRight && !pressingLeft)
    {
        players.front()->MoveAim(-2.0f);
    }

    /* Regular Loop for Updating Players */
    for (auto& player : players)
    {
        player->Update(deltaTime);
    }
    /* Regular Loop for Updating Bullets */
    for (auto& bullet : bullets)
    {
        bullet->Update(deltaTime);
    }
    /* Regular Loop for Updating Effects */
    for (auto& effect : effects)
    {
        effect->Update(deltaTime);
    }


    /* Removes and Deletes Players if they expire */
    players.erase(
        std::remove_if(players.begin(), players.end(),
                       [](const std::unique_ptr<Player>& e) { return e->IsExpired(); }),
        players.end()
    );


    /* Removes and Deletes bullets if they expire */
    // TODO Delete after demo.
    for (int i = 0; i < bullets.size(); i++)
    {
        if (bullets[i] != bullets[0])
        {
            Collision::CollisionResponse(*bullets[i], *bullets[i - 1]);
        }
    }
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const std::unique_ptr<Entity>& e) { return e->IsExpired(); }),
        bullets.end()
    );


    /* Removes and Deletes effects if they expire */
    effects.erase(
        std::remove_if(effects.begin(), effects.end(),
                       [](const std::unique_ptr<Entity>& e) { return e->IsExpired(); }),
        effects.end()
    );
}

void Scene0g::Render() const
{
    /// Set the background color then clear the screen
    glClearColor(0.33f, 0.0f, 0.33f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (drawInWireMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                       camera->GetProjectionMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera->GetViewMatrix());

    /** Render Main Player **/
    mainPlayerActor->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    glUseProgram(shader->GetProgram());


    //// Render the sprite
    // no but like actually this is all you need in the scene render function to render a sprite
    // look at the header file for more info but basically you need a sprite mesh and a sprite renderer
    // to animate a sprite sheet just pass in the current sprite index to the renderSprite function
    // so we still need to figure out how to animate the sprite sheet

    /* Regular Loop for Rendering Players */
    for (auto& player : players)
    {
        playerRenderer->renderSpriteSheet(shader, sprite_Mesh, player->GetModelMatrix(), 1);
        crossHairsRenderer->renderSprite(shader, sprite_Mesh, player->GetAimModelMatrix());
        player->DrawHitBox(camera->GetProjectionMatrix(), camera->GetViewMatrix(), sprite_Mesh);
        glUseProgram(shader->GetProgram());
    }

    /* Regular Loop for Rendering Bullets */
    for (auto& bullet : bullets)
    {
        bulletsRenderer->renderSpriteSheet(shader, sprite_Mesh, bullet->GetModelMatrix(), 1);
        bullet->DrawHitBox(camera->GetProjectionMatrix(), camera->GetViewMatrix(), sprite_Mesh);
        glUseProgram(shader->GetProgram());
    }

    /* Regular Loop for Rendering Effects */
    for (auto& effect : effects)
    {
        impactRenderer->renderSpriteSheet(shader, sprite_Mesh, effect->GetModelMatrix(), 1);
        effect->DrawHitBox(camera->GetProjectionMatrix(), camera->GetViewMatrix(), sprite_Mesh);
        glUseProgram(shader->GetProgram());
    }


    glUseProgram(0);
}
