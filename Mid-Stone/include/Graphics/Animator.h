#pragma once
#include <Graphics/AnimationClip.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <SDL_render.h>
class Animator{
private:
	//The name of the current clip being played
	std::string currentClipName;

	//Pointer to the current clip being played
	AnimationClip* currentClip;

	//Holds the animation clips in an unordered map for easy access via a name that is tied to the animation clip
	std::unordered_map<std::string, AnimationClip*> animationClips;

	bool isPlaying = false;
public:

	//Basic getter to retrieve the current clips name
	std::string getCurrentClipName() const { return currentClipName; }

	//Adds a new Animation clip to the animator using emplace (emplace adds new data into the map)
	void addAnimationClip(const std::string& name, AnimationClip* clip);

	//Plays an animation clip based off a name provided
	void playAnimationClip(const std::string& name);

	AnimationClip* getCurrentClip() const { return currentClip; }

	//calls update on the current clip (currently there is no update function in AnimationClip so this does nothing)
	void update(float deltaSeconds) {
		if (currentClip) {
			currentClip->update(deltaSeconds);
			std::cout << "Current frame " << currentClip->getCurrentFrame() << std::endl;
		}
	}


	
};

