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
	enum class SCENE_NUMBER {
		SCENE0g = 0,
		SCENE0p,
		SCENE1,
		SCENE2,
		SCENE3,
		SCENE4,
		SCENE5,
		SCENE6
	};

	Scene* currentScene;
	Timer* timer;

	Window* window;       // <-- legacy SDL/OpenGL window
	GuiWindow* imguiWin; // <-- new ImGui-enabled window

	unsigned int fps;
	bool isRunning;
	bool fullScreen;

	bool BuildNewScene(SCENE_NUMBER scene_);
};

#endif // SCENEMANAGER_H
