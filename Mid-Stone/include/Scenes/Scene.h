#ifndef SCENE_H
#define SCENE_H
#include "Core/SceneManager.h"

union SDL_Event;

/**
 * Defines the types of scene requests that can be made within the application.
 *
 * This enumeration is used to specify the type of operation a scene manager
 * or individual scene wants to perform. The available types allow for signaling
 * operations such as:
 * - No operation (`none`)
 * - Quitting the application (`quit_application`)
 * - Restarting the current scene (`restart_scene`)
 * - Changing to a different scene (`change_scene`)
 *
 * Each value in this enumeration corresponds to a specific request type, and it is
 * used in conjunction with the `SceneRequest` structure to encapsulate the details
 * of a request made by or directed to a scene.
 */
enum class Scene_request_type : uint8_t
{
    none = 0,
    quit_application,
    restart_scene,
    change_scene
};

/**
 * Represents a request to manage scene transitions or operations.
 *
 * This structure contains the information necessary for controlling the
 * state of scenes, including the type of request and the specific scene
 * being targeted. It is utilized to facilitate interactions between
 * the scene manager and individual scenes, enabling operations such as
 * transitioning to a new scene, restarting the current one, or exiting the application.
 */
struct SceneRequest
{
    /**
     * Represents the type of a request related to scene operations.
     *
     * This variable defines the specific type of request being made
     * within the context of scene handling, whether it is a request
     * to quit the application, restart the current scene, change to
     * another scene, or perform no action.
     */
    Scene_request_type type{Scene_request_type::none};
    /**
     * Specifies the target scene for a scene change operation.
     *
     * This variable represents a specific scene to transition to and is used in
     * conjunction with scene management requests such as changing scenes. It holds
     * an enumerated value of type `Scene_number`, which corresponds to a predefined
     * scene within the application (e.g., `scene_level_1`, `animation_scene`).
     */
    Scene_number targetScene{Scene_number::scene_level_1};
};


class Scene
{
    /**
     * Represents a request related to scene operations.
     *
     * This structure encapsulates information about a pending request for
     * scene management, including the request type and targeted scene number.
     * It is used to facilitate communication between the scene manager and
     * individual scenes for handling operations such as quitting the application,
     * restarting the current scene, or changing to a different scene.
     */
private:
    SceneRequest pendingRequest{};

    /**
     * Requests the application to quit by setting the pending scene request
     * type to `Scene_request_type::quit_application`.
     *
     * This method is protected and intended to be called within the class
     * or derived classes to signal that the application should terminate.
     */
protected:
    void RequestQuitApplication()
    {
        pendingRequest.type = Scene_request_type::quit_application;
    }

    /**
     * @brief Requests a restart of the current scene.
     *
     * This method sets the type of the pending scene request to `Scene_request_type::restart_scene`.
     * It is typically used when a user action or game event triggers the need to restart the scene.
     */
    void RequestRestartScene()
    {
        pendingRequest.type = Scene_request_type::restart_scene;
    }

    /**
     * Requests a scene change to the specified target scene.
     *
     * This function prepares a request to transition from the current scene
     * to the scene specified by the target parameter. The request is stored
     * in the `pendingRequest` member variable, with the request type set to
     * `Scene_request_type::change_scene` and the target scene defined by
     * the target parameter.
     *
     * @param target The target scene to transition to, represented by an
     *               enum value of type Scene_number.
     */
    void RequestChangeScene(Scene_number target)
    {
        pendingRequest.type = Scene_request_type::change_scene;
        pendingRequest.targetScene = target;
    }
public:
    bool levelFinished = false;

    virtual ~Scene()
    {
    }

    virtual bool OnCreate() = 0;
    virtual void OnDestroy() = 0;
    virtual void Update(const float deltaTime) = 0;
    virtual void Render() const = 0;
    virtual void HandleEvents(const SDL_Event& sdlEvent) = 0;
    virtual void RenderGUI() = 0;

    SceneRequest ConsumeSceneRequest()
    {
        const SceneRequest copy = pendingRequest;
        pendingRequest.type = Scene_request_type::none;
        return copy;
    }


};

#endif
