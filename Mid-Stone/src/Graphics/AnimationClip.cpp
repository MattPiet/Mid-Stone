#include "Graphics/AnimationClip.h"
#include <algorithm>
#include <iostream>

void AnimationClip::Update(float deltaSeconds)
{
    // If there are no frames do nothing
    if (totalFrames <= 0)
    {
        std::cout << "[AnimationClip] No frames to update." << std::endl;
        return;
    }

    // Adds time since last update
    secondsSinceLastUpdate += deltaSeconds;

    // Only update frame if enough time has passed
    if (secondsSinceLastUpdate >= frameDuration && !isFinished)
    {
        secondsSinceLastUpdate -= frameDuration; // subtract frame duration, preserve remainder

        // Playback logic
        switch (playMode)
        {
        case Play_mode::loop:
            if (currentFrame < startFrame) { currentFrame = startFrame; }

            currentFrame++;
            if (currentFrame >= endFrame)
            {
                currentFrame = 0;
                std::cout << "[AnimationClip] Looping animation." << std::endl;
            }

            break;
        case Play_mode::once:
            isFinished = false;
            currentFrame = std::max(currentFrame, startFrame);
            if (currentFrame <= endFrame)
            {
                currentFrame++;
                if (currentFrame > endFrame)
                {
                    currentFrame = endFrame - 1;
                    isFinished = true;
                    //  std::cout << "[AnimationClip] Animation finished (ONCE)." << std::endl;
                }
            }
            break;
        case Play_mode::reverse:
            isFinished = false;
            currentFrame--;
            if (currentFrame < startFrame)
            {
                currentFrame = endFrame - 1;
                isFinished = true;
                //  std::cout << "[AnimationClip] Playing animation in reverse." << std::endl;
            }
            break;
        case Play_mode::pingpong:
            if (isPlayingForward)
            {
                if (currentFrame < startFrame) { currentFrame = startFrame; }
                currentFrame++;
                if (currentFrame >= endFrame)
                {
                    currentFrame = endFrame - 1;
                    isPlayingForward = false;
                    // std::cout << "[AnimationClip] PingPong: reversing direction (backward)." << std::endl;
                }
            }
            if (!isPlayingForward)
            {
                currentFrame--;
                if (currentFrame < startFrame)
                {
                    currentFrame = startFrame;
                    isPlayingForward = true;
                    // std::cout << "[AnimationClip] PingPong: reversing direction (forward)." << std::endl;
                }
            }
            break;
        }
    }
}
