#include "Graphics/Animator.h"

void Animator::addAnimationClip(const std::string& name, AnimationClip* clip) {
	animationClips.emplace(name, clip);
	std::cout << "Added clip: " << name << std::endl;
}

void Animator::playAnimationClip(const std::string& name) {

	//Automatically iterates through the map to find the clip
	auto it = animationClips.find(name);

	//If found set the current clip to the found clip
	if (it != animationClips.end()) {
		std::cout << "Playing clip: " << name << std::endl;
		currentClipName = name;
		currentClip = it->second;
		currentClip->reset();
	}
	//If the clip is not found set the current clip to null and the name to an empty string and print that there was no clip found
	else {
		currentClipName = "";
		currentClip = nullptr;
		std::cout << "Clip not found!" << std::endl;
		throw std::runtime_error("Animation clip'" + name + "'not found.");
	}
}

void Animator::update(float deltaSeconds) {
	if (currentClip) {
		currentClip->update(deltaSeconds);
		//std::cout << "Current frame " << currentClip->getCurrentFrame() << std::endl;
		if (currentClip->getIsFinished()) {
			animationCallback();
		}
	}
}

void Animator::setDefaultAnimation(const std::string name) {
	if (animationClips.find(name) != animationClips.end()) {
		defaultClipName = name;
		std::cout << "Default animation set to \"" << name << "\"\n";
	}
	else {
		std::cerr << "Error: Default animation \"" << name << "\" not found!\n";
	}
}

void Animator::animationCallback() {
	if (currentClipName != defaultClipName) {
		if (currentClip && currentClip->getIsFinished()) {
			if (!defaultClipName.empty()) {
				playAnimationClip(defaultClipName);
				std::cout << "playing default animation" << std::endl;
			}
		}
	}
}
