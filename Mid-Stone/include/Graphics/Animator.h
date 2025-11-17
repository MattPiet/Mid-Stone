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

	//Holds the name of the default animation clip
	std::string defaultClipName;

	bool isPlaying = false;
public:

	~Animator() {
		//Destructor to clean up the animation clips
		for (auto& pair : animationClips) {
			delete pair.second;
		}
		animationClips.clear();
	}
	//Basic getter to retrieve the current clips name
	std::string getCurrentClipName() const { return currentClipName; }

	//Adds a new Animation clip to the animator using emplace (emplace adds new data into the map)
	void addAnimationClip(const std::string& name, AnimationClip* clip);

	void addDefaultAnimationClip(const std::string& name, AnimationClip* clip) {addAnimationClip(name, clip);setDefaultAnimation(name);}

	//Plays an animation clip based off a name provided
	void playAnimationClip(const std::string& name);

	AnimationClip* getCurrentClip() const { return currentClip; }

	//calls update on the current clip (currently there is no update function in AnimationClip so this does nothing)
	void update(float deltaSeconds);

	//Sets the default animation clip by name
	void setDefaultAnimation(const std::string name);

	//Callback function to be called when an animation clip finishes playing
	void animationCallback();
	
};

