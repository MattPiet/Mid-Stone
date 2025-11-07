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
    mainPlayerActor = std::make_unique<Actor2D>();
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

    /** Set up Main Player Controller **/
    mainPlayerController = std::make_unique<PlayerController>();

    mainPlayerController->OnCreate("sprites/crosshairs.png");
    mainPlayerController->SetPossessedActor(mainPlayerActor.get());

    /** Renderer Setup **/
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

	ImGuiIO& io = ImGui::GetIO(); // access the ImGuiIO structure
	io.Fonts->AddFontDefault(); // add default font

  
    // we can use this to draw stuff striaght to the screen using ImGui
    
	MainFont = io.Fonts->AddFontFromFileTTF("Fonts/times.ttf", 18.5f); // load a custom font



    return true;
}

void Scene0g::OnDestroy()
{
    Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
    /** Delete Main Player **/
    mainPlayerActor.reset();
    mainPlayerController.reset();

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
    

    delete impactRenderer;
    impactRenderer = nullptr;


    cameraController.reset();
    camera.reset();

    for (auto& player : players)
    {
		player.reset();
    }

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
                bullet->SetPosition(Vec3(30.0f, 20.0f, 0));
                bullet->AdjustOrientation(QMath::angleAxisRotation(45.0f, MATH::Vec3(0.0f, 0.0f, 1.0f)));
				bullet->SetVelocity(Vec3(-50.0f, 0.0f, 0.0f));
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
        case SDL_SCANCODE_P:
            if (auto bullet = players.front()->Shoot())
            {
                bullet->SetPosition(Vec3(-30.0f, 15.0f, 0));
				bullet->AdjustOrientation(QMath::angleAxisRotation(45.0f, MATH::Vec3(0.0f, 0.0f, 1.0f)));
				bullet->SetVelocity(Vec3(50.0f, 0.0f, 0.0f));
                bullet->OnCreate(bulletsRenderer);
                bullet->AdjustHitboxSize(Vec3{ -10.f, -10.f, 0.0f });
                // implicit upcast unique_ptr<Bullet> -> unique_ptr<Entity> (default deleter)
                bullet->SetExpiredCallback([this](Entity& e)
                    {
                        auto impact = std::make_unique<Entity>(e.GetPosition(), Vec3{ 1.0f, 1.0f, 1.0f }, Hit_box_type::quad);
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
        mainPlayerController->MoveAim(2.0f);
    }
    if (pressingRight && !pressingLeft)
    {
        mainPlayerController->MoveAim(-2.0f);
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
		bullets[i]->UpdatePhysics(deltaTime);
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

	
    ImGui::SetNextWindowPos(ImVec2(0, 100)); // (x, y) in screen coordinates
    ImGui::SetNextWindowBgAlpha(0.0f); // Fully transparent background (0 = invisible, 1 = opaque)
	ImGui::Begin("##HiddenLabel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize); // this basically sets the window to be invisible
	if (ImGui::Button("Draw HitBox")) { // making a button to toggle hitbox drawing
		mainPlayerActor->draw_Hitbox = !mainPlayerActor->draw_Hitbox;
	}
	ImGui::End();// we end the invisible window here
	ImGui::SetNextWindowBgAlpha(150.0f); // reset back to opaque for other windows
	

	ImDrawList* drawList = ImGui::GetBackgroundDrawList(); // Get the background draw list to draw behind all ImGui windows
	drawList->AddRectFilled(ImVec2(10, 10), ImVec2(400, 100), IM_COL32(0, 0, 0, 150)); // Draw a semi-transparent rectangle as background for text
	drawList->AddText(MainFont, 22.0f, ImVec2(20, 60), IM_COL32(0, 255, 0, 255), "Hello Im a different font then the rest!"); // Draw text with custom font and size
	drawList->AddText(ImVec2(20, 20), IM_COL32(255, 255, 255, 255), "Hello from ImGui!"); // Draw text at position (20, 20)
	drawList->AddText(ImVec2(20, 40), IM_COL32(255, 255, 0, 255), "Players active: 1"); // Draw text at position (20, 40)

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
    /** Render Main Controller **/
    mainPlayerController->RenderCrossHairs(camera->GetViewMatrix(), camera->GetProjectionMatrix());

    

    glUseProgram(shader->GetProgram());
    

    /* Regular Loop for Rendering Players */
    for (auto& player : players)
    {
        playerRenderer->renderSpriteSheet(shader, sprite_Mesh, player->GetModelMatrix(), 1);
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
