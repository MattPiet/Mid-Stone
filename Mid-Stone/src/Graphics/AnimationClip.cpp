#include "Graphics/AnimationClip.h"
#include <iostream>
using namespace std;

void AnimationClip::update(float deltaSeconds) {
    // If there are no frames do nothing
    if (totalFrames <= 0) {
        cout << "[AnimationClip] No frames to update." << endl;
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
                cout << "[AnimationClip] Looping animation." << endl;
            }
            break;
        case PlayMode::ONCE:
            currentFrame++;
            if (currentFrame >= totalFrames) {
                currentFrame = totalFrames - 1;
                isFinished = true;
                cout << "[AnimationClip] Animation finished (ONCE)." << endl;
            }
            break;
        case PlayMode::REVERSE:
            currentFrame--;
            if (currentFrame < 0) {
                currentFrame = totalFrames - 1;
               cout << "[AnimationClip] Playing animation in reverse." << endl;
            }
            break;
        case PlayMode::PINGPONG:
            if (isPlayingForward) {
                currentFrame++;
                if (currentFrame >= totalFrames) {
                    currentFrame = totalFrames - 2;
                    isPlayingForward = false;
                    cout << "[AnimationClip] PingPong: reversing direction (backward)." << endl;
                }
            }
            else {
                currentFrame--;
                if (currentFrame < 0) {
                    currentFrame = 1;
                    isPlayingForward = true;
                    cout << "[AnimationClip] PingPong: reversing direction (forward)." << endl;
                }
            }
            break;
        }
    }
}
