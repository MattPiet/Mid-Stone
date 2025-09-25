#include <glew.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <Scenes/AnimationScene.h>
#include <MMath.h>
#include <Utils/Debug.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Entities/Body.h>

#include <SDL_mixer.h>

///ImGui includes
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

AnimationScene::AnimationScene(){
	Debug::Info("Created AnimationScene: ", __FILE__, __LINE__);
}

AnimationScene::~AnimationScene() {
	Debug::Info("Deleted AnimationScene: ", __FILE__, __LINE__);
}

bool AnimationScene::OnCreate() {
	Debug::Info("Loading assets AnimationScene: ", __FILE__, __LINE__);
	return true;
}

void AnimationScene::OnDestroy() {
	Debug::Info("Deleting assets AnimationScene: ", __FILE__, __LINE__);
}

void AnimationScene::HandleEvents(const SDL_Event& sdlEvent) {
	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:

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

void AnimationScene::RenderGUI() {
}

void AnimationScene::Update(const float deltaTime) {
}

void AnimationScene::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(0);
}




