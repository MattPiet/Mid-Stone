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
		scene0_p,
		scene1,
		scene2,
		scene3,
		scene4,
		scene5,
		scene6
	};

	Scene* currentScene;
	Timer* timer;

	Window* window;       // <-- legacy SDL/OpenGL window
	GuiWindow* imguiWin; // <-- new ImGui-enabled window

	unsigned int fps;
	bool isRunning;
	bool fullScreen;

	bool BuildNewScene(Scene_number scene_);
};

#endif // SCENEMANAGER_H
