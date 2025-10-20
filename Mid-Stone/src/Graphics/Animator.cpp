#include "Graphics/Animator.h"

void Animator::addAnimationClip(const string& name, AnimationClip* clip) {
	animationClips.emplace(name, clip);
	cout << "Added clip: " << name << endl;
}

void Animator::playAnimationClip(const string& name) {

	//Automatically iterates through the map to find the clip
	auto it = animationClips.find(name);

	//If found set the current clip to the found clip
	if (it != animationClips.end()) {
		cout << "Playing clip: " << name << endl;
		currentClipName = name;
		currentClip = it->second;
		currentClip->reset();
		return;
	}
	//If the clip is not found set the current clip to null and the name to an empty string and print that there was no clip found
	else {
		currentClipName = "";
		currentClip = nullptr;
		cout << "Clip not found!" << endl;
		throw runtime_error("Animation clip'" + name + "'not found.");
	}
}
