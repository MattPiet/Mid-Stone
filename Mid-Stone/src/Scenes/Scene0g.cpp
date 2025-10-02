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

///ImGui includes
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//#include <Utils/MemoryMonitor.h>

unsigned int shaderProgram;

Scene0g::Scene0g() : sphere{ nullptr }, shader{ nullptr }, mesh{ nullptr },
drawInWireMode{ false }, master_volume{ 1.0f }, mixer{ nullptr }, sprite_Mesh{ nullptr }, sprite_Renderer{ nullptr }
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

    sprite_Mesh = new SpriteMesh();
    sprite_Mesh->OnCreate();

    sprite_Renderer = new SpriteRenderer();
    sprite_Renderer->loadImage("sprites/fatty_clicked.png");

    shader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
    if (!shader->OnCreate())
    {
        std::cout << "Shader failed ... we have a problem\n";
    }

    projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
    viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    modelMatrix.loadIdentity();


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

    sprite_Mesh->OnDestroy();
    delete sprite_Mesh;
    sprite_Mesh = nullptr;

	delete sprite_Renderer;
    sprite_Renderer = nullptr;

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
    if (ImGui::SliderFloat("Sphere Scale", &sphereScale, 0.1f, 5.0f))
    {
        modelMatrix = MMath::scale(sphereScale, sphereScale, sphereScale);
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
    glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
    glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrix);

	//// Render the sprite
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_Renderer->getTextureID());
	std::pair sprite_info = sprite_Renderer->runSpriteSheet(1, 1, 0); // rows, columns, current_sprite_index

    glUniform1i(shader->GetUniformID("ourTexture"), 0);
    glUniform2f(shader->GetUniformID("uvScale"), sprite_info.first.first, sprite_info.first.second);
    glUniform2f(shader->GetUniformID("uvOffset"), sprite_info.second.first, sprite_info.second.second);

    sprite_Mesh->RenderMesh();
     //mesh->Render(GL_TRIANGLES);
    glUseProgram(0);




}




	
