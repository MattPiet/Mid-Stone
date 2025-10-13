#pragma once
#include "AnimationClip.h"
#include <string>
#include <unordered_map>
#include <iostream>
using namespace std;
class Animator{
private:
	//The name of the current clip being played
	string currentClipName;

	//Pointer to the current clip being played
	AnimationClip* currentClip;

	//Holds the animation clips in an unordered map for easy access via a name that is tied to the animation clip
	std::unordered_map<string, AnimationClip*> animationClips;
public:

	//Basic getter to retrieve the current clips name
	string getCurrentClipName() const { return currentClipName; }


	//Adds a new Animation clip to the animator using emplace (emplace adds new data into the map)
	void addAnimationClip(const string& name, AnimationClip* clip) {animationClips.emplace(name, clip);}


	//Plays an animation clip based off a name provided
	void playAnimationClip(const string& name) {

		//Automatically iterates through the map to find the clip
		auto it = animationClips.find(name);

		//If found set the current clip to the found clip
		if (it != animationClips.end()) {
			currentClipName = name;
			currentClip = it->second;
			currentClip->resetFrame();
		}
		//If the clip is not found set the current clip to null and the name to an empty string and print that there was no clip found
		else {
			currentClipName = "";
			currentClip = nullptr;
			cout << "Clip not found!" << endl;
			throw runtime_error("Animation clip'" + name + "'not found.");
		} 
	}
};

