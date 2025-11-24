#pragma once
#include <Graphics/AnimationClip.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <SDL_render.h>

class Animator
{
private:
    //The name of the current clip being played
    std::string currentClipName;

    //Pointer to the current clip being played
    AnimationClip* currentClip = nullptr;

    //Holds the animation clips in an unordered map for easy access via a name that is tied to the animation clip
    std::unordered_map<std::string, AnimationClip*> animationClips;

    //Holds the name of the default animation clip
    std::string defaultClipName;
    
public:
    /**
     * @brief Destructor for the Animator class.
     *
     * Cleans up and deallocates the memory used by the stored animation clips.
     * Iterates through the internal unordered map of animation clips, deletes each
     * dynamically allocated AnimationClip pointer, and clears the map to ensure
     * no memory leaks during the destruction of the Animator object.
     */
    ~Animator()
    {
        //Destructor to clean up the animation clips
        for (auto& pair : animationClips)
        {
            delete pair.second;
        }
        animationClips.clear();
    }

    /**
     * Retrieves the name of the current animation clip being played.
     *
     * @return A string representing the name of the currently playing animation clip.
     */
    [[nodiscard]] std::string GetCurrentClipName() const { return currentClipName; }

    /**
     * Adds an animation clip to the animator with a specified name for identification.
     *
     * @param name The name of the animation clip to be added.
     * @param clip Pointer to the animation clip object.
     */
    void AddAnimationClip(const std::string& name, AnimationClip* clip);

    /**
     * Adds an animation clip and sets it as the default animation clip.
     *
     * This method first adds the provided animation clip to the collection
     * of clips using the name as the identifier, and then sets it as the
     * default animation to be played.
     *
     * @param name The name to associate with the animation clip.
     * @param clip Pointer to the AnimationClip object to be added and set as default.
     */
    void AddDefaultAnimationClip(const std::string& name, AnimationClip* clip)
    {
        AddAnimationClip(name, clip);
        SetDefaultAnimation(name);
    }

    /**
     * Plays the animation clip associated with the given name. If the specified
     * animation clip is not found, an exception is thrown.
     *
     * @param name The name of the animation clip to play.
     * @throws std::runtime_error If the animation clip with the specified name is not found.
     */
    void PlayAnimationClip(const std::string& name);

    /**
     * Retrieves the current animation clip being played by the animator.
     * This function provides a pointer to the animation clip currently active,
     * allowing access to details such as frames, play mode, and other animation properties.
     *
     * @return A pointer to the current AnimationClip object. Returns nullptr if no clip is currently active.
     */
    [[nodiscard]] AnimationClip* GetCurrentClip() const { return currentClip; }

    /**
     * Retrieves a pointer to an AnimationClip associated with the specified name.
     *
     * @param name The name of the animation clip to retrieve.
     * @return A pointer to the AnimationClip if found, otherwise nullptr.
     */
    [[nodiscard]] AnimationClip* GetAnimationClip(const std::string& name) const
    {
        const auto it = animationClips.find(name);
        return it != animationClips.end() ? it->second : nullptr;
    }

    /**
     * Updates the animation state by progressing the current animation clip
     * according to the elapsed time and checking if the animation has reached
     * the end. If the animation finishes, it triggers the AnimationCallback function.
     *
     * @param deltaSeconds The time elapsed since the last update, in seconds.
     */
    void Update(float deltaSeconds);

    /**
     * Sets the default animation by specifying its name.
     * If the specified animation name exists in the collection of animation clips,
     * it sets the animation as the default. Otherwise, it logs an error message.
     *
     * @param name The name of the animation clip to be set as default.
     */
    void SetDefaultAnimation(const std::string name);

    /**
     * Handles the logic for transitioning to the default animation clip once the current animation clip finishes playing.
     *
     * This method checks if the `currentClip` has completed its playback and determines whether to switch to the default animation clip.
     * If the current animation clip is finished and a default animation clip has been specified, the default animation clip is played.
     *
     * Key Behavior:
     * - If the `currentClipName` is not the same as the `defaultClipName` and the `currentClip` is finished,
     *   the method transitions to the default animation clip by invoking `PlayAnimationClip` with `defaultClipName`.
     * - Logs a message indicating that the default animation clip is being played.
     */
    void AnimationCallback();
};
