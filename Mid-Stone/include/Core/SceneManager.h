#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <string>



// Forward declarations
class Scene;
class Timer;
class Window;
class GuiWindow;

enum class Scene_number : uint8_t {
	scene0_g = 0,
	scene_level_1,
	scene_level_2,
	scene_level_3,
	animation_scene,
	MainMenu
};

class SceneManager {
public:
	SceneManager();
	~SceneManager();
	void Run();
	bool Initialize(std::string name_, int width_, int height_);
	void HandleEvents();
	static bool GetAudioStateStatic() { return setAudioOn; }
	static float GetMasterVolumeStatic() { return master_volume; } // Placeholder for actual volume retrieval
	static void SetMasterVolumeStatic(float volume) { master_volume = volume; } // Placeholder for actual volume setting
private:
	static bool setAudioOn; 
	static float master_volume;

	Scene* currentScene;
	Scene_number currentSceneId{ Scene_number::scene_level_2 };
	Timer* timer;

	Window* window;       // <-- legacy SDL/OpenGL window
	GuiWindow* imguiWin; // <-- new ImGui-enabled window

	unsigned int fps;
	bool isRunning;
	bool fullScreen;

	bool BuildNewScene(Scene_number scene_);
	/**
	 * Handles pending scene requests and processes them based on their type.
	 *
	 * This method checks the current scene for any requests related to scene
	 * transitions or application-level operations and takes appropriate action.
	 * The possible requests include:
	 *
	 * - `Scene_request_type::none`: No action is taken.
	 * - `Scene_request_type::quit_application`: Signals the application to stop running.
	 * - `Scene_request_type::restart_scene`: Restarts the current scene by
	 *   rebuilding it with the same scene ID.
	 * - `Scene_request_type::change_scene`: Transitions to a different scene
	 *   as specified by the request's `targetScene`.
	 *
	 * If no current scene is active (i.e., `currentScene` is `nullptr`), the method
	 * returns without performing any operations.
	 */
	void HandleSceneRequest();

	void DestroyScene();
};

#endif // SCENEMANAGER_H


