#include <glew.h>

#include "Scenes/SceneLevel1.h"
#include "Utils/Debug.h"


SceneLevel1::SceneLevel1()
{
    Debug::Info("Created Scene Level 1: ", __FILE__, __LINE__);
}

SceneLevel1::~SceneLevel1()
{
    Debug::Info("Deleted Scene Level 1: ", __FILE__, __LINE__);
}


void SceneLevel1::Update(const float deltaTime)
{
}

void SceneLevel1::HandleEvents(const SDL_Event& sdlEvent)
{
}

bool SceneLevel1::OnCreate()
{
    return true;
}

void SceneLevel1::RenderGUI()
{
}

void SceneLevel1::OnDestroy()
{
}

void SceneLevel1::Render() const
{
    /** Render Setup **/
    glClearColor(0.541f, 0.765f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(0);
}
