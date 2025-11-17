#include "Graphics/AnimationClip.h"
#include <iostream>

void AnimationClip::update(float deltaSeconds) {
    // If there are no frames do nothing
    if (totalFrames <= 0) {
        std::cout << "[AnimationClip] No frames to update." << std::endl;
        return;
    }

    // Adds time since last update
    secondsSinceLastUpdate += deltaSeconds;

    // Only update frame if enough time has passed
    if (secondsSinceLastUpdate >= frameDuration && !isFinished) {
        secondsSinceLastUpdate -= frameDuration; // subtract frame duration, preserve remainder

        // Playback logic
        switch (playMode) {
        case PlayMode::LOOP:
           
                currentFrame++;
                if (currentFrame >= totalFrames) {
                    currentFrame = 0;
                   // std::cout << "[AnimationClip] Looping animation." << std::endl;
                }
            
            break;
        case PlayMode::ONCE:
			isFinished = false;
            if (currentFrame <= totalFrames) {
                currentFrame++;
                if (currentFrame >= totalFrames) {
                    currentFrame = totalFrames - 1;
                    isFinished = true;
                  //  std::cout << "[AnimationClip] Animation finished (ONCE)." << std::endl;
                }
            }
            break;
        case PlayMode::REVERSE:
            isFinished = false;
            currentFrame--;
            if (currentFrame < 0) {
                currentFrame = totalFrames - 1;
				isFinished = true;
             //  std::cout << "[AnimationClip] Playing animation in reverse." << std::endl;
            }
            break;
        case PlayMode::PINGPONG:
            if (isPlayingForward) {
                currentFrame++;
                if (currentFrame >= totalFrames) {
                    currentFrame = totalFrames - 1;
                    isPlayingForward = false;
                   // std::cout << "[AnimationClip] PingPong: reversing direction (backward)." << std::endl;
                }
            }
            if (!isPlayingForward) {
                currentFrame--;
                if (currentFrame < 0) {
                    currentFrame = 1;
                    isPlayingForward = true;
                   // std::cout << "[AnimationClip] PingPong: reversing direction (forward)." << std::endl;
                }
            }
            break;
        }
    }
}


