#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <string>



// Forward declarations
class Scene;
class Timer;
class Window;
class GuiWindow;  // <-- add this

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void Run();
	bool Initialize(std::string name_, int width_, int height_);
	void HandleEvents();

private:
	enum class Scene_number : uint8_t {
		scene0_g = 0,
		scene_level_1,
		scene_level_2,
		animation_scene,
	};

	Scene* currentScene;
	Timer* timer;

	Window* window;       // <-- legacy SDL/OpenGL window
	GuiWindow* imguiWin; // <-- new ImGui-enabled window

	unsigned int fps;
	bool isRunning;
	bool fullScreen;

	bool BuildNewScene(Scene_number scene_);

	void DestroyScene();
};

#endif // SCENEMANAGER_H
