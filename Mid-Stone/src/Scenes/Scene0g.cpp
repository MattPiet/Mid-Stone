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

Scene0g::Scene0g() : sphere{nullptr}, shader{nullptr}, mesh{nullptr},
                     drawInWireMode{false}, master_volume{1.0f}, mixer{nullptr}, sprite_Mesh{nullptr},
                     sprite_Renderer{nullptr},
                     spriteSheet_Renderer{nullptr}
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
    sphere = new Body();
    sphere->OnCreate();

    mesh = new Mesh("meshes/Sphere.obj");
    mesh->OnCreate();

	fistEntity = new Entity(Vec3(44.0f, 36.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), 'q');

    // Create the sprite mesh and sprite renderer //// look in the headr file for more info
    sprite_Mesh = new SpriteMesh();
    sprite_Mesh->OnCreate();

    sprite_Renderer = new SpriteRenderer();
    sprite_Renderer->loadImage("sprites/fist.png", 2, 8);
	fistEntity->OnCreate(sprite_Renderer);

    spriteSheet_Renderer = new SpriteRenderer();
    spriteSheet_Renderer->loadImage("sprites/idle.png", 1, 3);

    shader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
    if (!shader->OnCreate())
    {
        std::cout << "Shader failed ... we have a problem\n";
    }

	

    projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);

    /// Set up the sprite projection matrix //// This is orthographic for 2D rendering
    spriteProjectionMatrix = MMath::orthographic(0.0f, 1280.0f / 10.0f,
                                                 0.0f, 720.0f / 10.0f,
                                                 -1.0f, 1.0f);

    viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    modelMatrix.loadIdentity();
    /// Move the sprite to the center of the screen ish
    modelMatrix = MMath::translate(44.0f, 36.0f, 0.0f);

    spriteSheet_ModelMatrix.loadIdentity();
    spriteSheet_ModelMatrix = MMath::translate(84.0f, 36.0f, 0.0f);

    SDL_Init(SDL_INIT_AUDIO);
    MIX_Init();

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

    if (!mixer)
    {
        std::cout << "Failed to create mixer: %s\n", SDL_GetError();
        return 0;
    }

    //// Load and play music
    MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/CrabRaave.wav", true);
    MIX_SetMasterGain(mixer, master_volume);
    MIX_PlayAudio(mixer, Music);
    MIX_DestroyAudio(Music);


    clip2 = new AnimationClip( AnimationClip::PlayMode::PINGPONG, 1.0f, 2, 2);
	animator = new Animator();
	animator->addAnimationClip("Idle", clip2);
	animator->addAnimationClip("Default", clip1);
	animator->playAnimationClip("Idle");


    //// Load and play sound

    //MIX_Audio* sound = MIX_LoadAudio(mixer, "Audio/laser.wav", true);
    /*if (!sound) {
        std::cout << "Failed to load audio: %s\n", SDL_GetError();
        MIX_DestroyMixer(mixer);
        return 0;
    MIX_PlayAudio(mixer, sound); // Play sound once
    SDL_Delay(400); // wait for sound to play ///// this is lowkey terrible but its for testing
    MIX_DestroyAudio(sound);
    MIX_DestroyMixer(mixer);
    MIX_Quit();
    }*/


    /** Renderer Setup **/
    crossHairsRenderer = new SpriteRenderer();
    crossHairsRenderer->loadImage("sprites/crosshairs.png");
    playerRenderer = new SpriteRenderer();
    playerRenderer->loadImage("sprites/idle.png", 1, 3);
    bulletsRenderer = new SpriteRenderer();
    bulletsRenderer->loadImage("sprites/fist.png", 2, 8);
    impactRenderer = new SpriteRenderer();
    impactRenderer->loadImage("sprites/impact.png", 2, 4);
    players.emplace_back(std::make_unique<Player>(Vec3{15, 15, 0}, Vec3{1.5f, 1.5f, 1.5f}, 'q'));
    for (auto& player : players) {
		player->OnCreate(playerRenderer);
    }

    return true;
}

void Scene0g::OnDestroy()
{
    Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
    sphere->OnDestroy();
    delete sphere;
    sphere = nullptr;

    mesh->OnDestroy();
    delete mesh;
    mesh = nullptr;

    shader->OnDestroy();
    delete shader;
    shader = nullptr;

	fistEntity->OnDestroy();
	delete fistEntity;
	fistEntity = nullptr;

    sprite_Mesh->OnDestroy();
    delete sprite_Mesh;
    sprite_Mesh = nullptr;

    delete sprite_Renderer;
    sprite_Renderer = nullptr;

    delete spriteSheet_Renderer;
    spriteSheet_Renderer = nullptr;

    delete playerRenderer;
    playerRenderer = nullptr;

    delete bulletsRenderer;
    bulletsRenderer = nullptr;

    delete crossHairsRenderer;
    crossHairsRenderer = nullptr;

    delete impactRenderer;
    impactRenderer = nullptr;

	delete animator;
	animator = nullptr;

	delete clip1;
	clip1 = nullptr;

	delete clip2;
	clip2 = nullptr;

    //// Turn off audio
    if (mixer)
    {
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }

    ///Deleting shaders
    for (auto& player : players) {
            player->OnDestroy();
    }

    for (auto& bullet : bullets) {
        bullet->OnDestroy();
	}

    for (auto& effect : effects) {
        effect->OnDestroy();
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
                // implicit upcast unique_ptr<Bullet> -> unique_ptr<Entity> (default deleter)
                bullet->SetExpiredCallback([this](Entity& e)
                {
                    auto impact = std::make_unique<Entity>(e.GetPosition(), Vec3{1.0f, 1.0f, 1.0f}, 'q');
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
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
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

    
static float sphereScale = 1.0f;
    if (ImGui::SliderFloat("Sphere Scale", &sphereScale, 0.9f, 1.1f))
    {
        modelMatrix *= MMath::scale(sphereScale, sphereScale, 0);
    }
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
    animator->update(deltaTime);  

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

    /*Updating Collisions*/
	for (int i = 0; i < bullets.size(); i++)
    {
        if (bullets[i] != bullets.back()) {
        
            if (Collision::CheckQuadQuadCollision(*bullets[i], *bullets[i + 1])) {
                Collision::CollisionResponse(*bullets[i], *bullets[i + 1]);
            }
            
        }
        
        
	}


    /* Removes and Deletes Players if they expire */
    for (auto& player : players)
    {
        if (player->IsExpired())
        {
            player->OnDestroy();
        }
    }
    players.erase(
        std::remove_if(players.begin(), players.end(),
                       [](const std::unique_ptr<Player>& e) { return e->IsExpired(); }),
        players.end()
    );
    

    /* Removes and Deletes bullets if they expire */
    for (auto& bullet : bullets)
    {
       if (bullet->IsExpired())
       {
           bullet->OnDestroy();
	   }
	}
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const std::unique_ptr<Entity>& e) { return e->IsExpired(); }),
        bullets.end()
    );
    

    /* Removes and Deletes effects if they expire */
    for (auto& effect : effects)
    {
        if (effect->IsExpired())
        {
            effect->OnDestroy();
        }
    }
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
    glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, spriteProjectionMatrix);
    //// Render the sprite
    // no but like actually this is all you need in the scene render function to render a sprite
    // look at the header file for more info but basically you need a sprite mesh and a sprite renderer
    // to animate a sprite sheet just pass in the current sprite index to the renderSprite function
    // so we still need to figure out how to animate the sprite sheet

	spriteSheet_Renderer->renderSprite(shader, sprite_Mesh, spriteSheet_ModelMatrix, animator->getCurrentClip()->getCurrentFrame()); // current_sprite_index

    fistEntity->DrawHitBox(spriteProjectionMatrix, sprite_Mesh);
    
    glUseProgram(shader->GetProgram());
    /* Regular Loop for Rendering Players */
    for (auto& player : players)
    {
        playerRenderer->renderSprite(shader, sprite_Mesh, player->GetModelMatrix());
        crossHairsRenderer->renderSprite(shader, sprite_Mesh, player->GetAimModelMatrix());
        player->DrawHitBox(spriteProjectionMatrix, sprite_Mesh);
        glUseProgram(shader->GetProgram());
    }

    /* Regular Loop for Rendering Bullets */
    for (auto& bullet : bullets)
    {
        bulletsRenderer->renderSprite(shader, sprite_Mesh, bullet->GetModelMatrix());
        bullet->DrawHitBox(spriteProjectionMatrix, sprite_Mesh);
        glUseProgram(shader->GetProgram());
    }

    /* Regular Loop for Rendering Effects */
    for (auto& effect : effects)
    {
        impactRenderer->renderSprite(shader, sprite_Mesh, effect->GetModelMatrix());
        effect->DrawHitBox(spriteProjectionMatrix, sprite_Mesh);
        glUseProgram(shader->GetProgram());
    }


    spriteSheet_Renderer->renderSprite(shader, sprite_Mesh, spriteSheet_ModelMatrix, 0); // current_sprite_index

    glUseProgram(0);
}
