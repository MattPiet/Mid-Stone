#pragma once
#include <cstdint>

class AnimationClip
{
public:
    enum class Play_mode : uint8_t
    {
        once,
        loop,
        pingpong,
        reverse
    };

private:
    // The playback mode of the animation clip (determines if it loops or plays once etc.)  
    Play_mode playMode;
    // Duration of each frame in seconds  
    float frameDuration;
    // The total number of frames in the clip  
    int totalFrames;
    // The current frame in the clip  
    int currentFrame;
    float secondsSinceLastUpdate;
    bool isPlayingForward;
    bool isFinished;

    /** First frame = 0 **/
    int startFrame;
    int endFrame;

public:
    // Default constructor which sets the frame duration to 0.1 with a default playmode to loop  
    AnimationClip() :
        playMode{Play_mode::loop},
        frameDuration{0.1f},
        totalFrames{1},
        currentFrame{0},
        secondsSinceLastUpdate{0.0f},
        isPlayingForward{true},
        isFinished{false},
        startFrame{0},
        endFrame{totalFrames}

    {
    }

    //dynamic constructor 
    explicit AnimationClip(Play_mode playMode_ = Play_mode::once, float frameDuration_ = 0.1f, int rows_ = 1,
                           int columns_ = 1,
                           int startFrame_ = 0, int endFrame_ = 1) :
        playMode{playMode_},
        frameDuration{frameDuration_},
        totalFrames{(columns_ * rows_)},
        startFrame{startFrame_},
        endFrame{endFrame_},
        currentFrame{startFrame_},
        secondsSinceLastUpdate{0.0f},
        isPlayingForward{true},
        isFinished{false}
    {
    }

    /**
     * Retrieves the duration of each animation frame in seconds.
     * @return The duration of one frame in seconds.
     */
    [[nodiscard]] float GetFrameDuration() const { return frameDuration; }

    /**
     * Retrieves the playback mode of the animation clip.
     *
     * @return The playback mode of the animation clip, represented as a value of the Play_mode enumeration.
     */
    [[nodiscard]] Play_mode GetPlayMode() const { return playMode; }

    /**
     * Retrieves the total number of frames in the animation clip.
     *
     * @return The total number of frames.
     */
    [[nodiscard]] int GetTotalFrames() const { return totalFrames; }

    /**
     * Retrieves the current frame of the animation clip.
     *
     * @return The index of the current frame within the animation clip.
     */
    [[nodiscard]] int GetCurrentFrame() const { return currentFrame; }

    /**
     * Returns the time elapsed, in seconds, since the last update.
     *
     * @return The time in seconds since the last update.
     */
    [[nodiscard]] float GetSecondsSinceLastUpdate() const { return secondsSinceLastUpdate; }

    /**
     * Indicates whether the animation is currently playing in a forward direction.
     * @return True if the animation is playing forward, otherwise false.
     */
    [[nodiscard]] bool GetIsPlayingForward() const { return isPlayingForward; }

    /**
     * Indicates whether the animation clip has finished playing.
     *
     * @return True if the animation clip has finished playing, false otherwise.
     */
    [[nodiscard]] bool GetIsFinished() const { return isFinished; }

    /**
     * Calculates and returns the total duration of the animation.
     *
     * @return The total duration of the animation in seconds, calculated as the frame duration multiplied by the total number of frames between startFrame and endFrame, inclusive.
     */
    [[nodiscard]] float GetAnimationDuration() const
    {
        return frameDuration * (static_cast<float>(endFrame) - static_cast<float>(startFrame) + 1);
    }

    /**
     * Resets the state of the animation clip.
     *
     * This method resets the current frame to the start frame,
     * clears the elapsed time since the last update,
     * sets the play direction to forward,
     * and marks the clip as not finished.
     */
    void Reset()
    {
        currentFrame = startFrame;
        secondsSinceLastUpdate = 0.0f;
        isPlayingForward = true;
        isFinished = false;
    }

    /**
     * Updates the animation clip by incrementing the frame based on the elapsed time.
     * The function determines the next frame to display depending on the play mode and elapsed time.
     * If there are no frames, the update is skipped.
     *
     * @param deltaSeconds The time in seconds since the last update.
     */
    void Update(float deltaSeconds);
};
