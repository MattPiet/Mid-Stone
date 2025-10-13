#pragma once
class AnimationClip{
public:
	enum class PlayMode {
		ONCE,
		LOOP,
		PINGPONG,
		REVERSE
	};
private:
	//Duration of each frame in seconds
	float frameDuration;
	//the playback mode of the animation clip (determines if it loops or plays once etcP)
	PlayMode playMode;
	//the current frame in the clip
	int currentFrame;
	//the total number of frames in the clip
	int totalFrames;
public:
	//Default constructor which sets the frame duration to 0.1 with a default playmode to loop
	AnimationClip() : frameDuration{ 0.1f }, playMode{ PlayMode::LOOP }, currentFrame{ 0 } {}
	
	//Dynamic constructor to fill in values 
	AnimationClip(float frameDuration_, PlayMode playMode_): frameDuration{ frameDuration_ }, playMode{ playMode_ } {}
	
	//Basic getter to retrieve the frame duration
	float getFrameDuration() const { return frameDuration; }


	//Basic getter to retrieve the playback mode of the animation
	PlayMode getPlayMode() const { return playMode; }

	void resetFrame() { currentFrame = 0; }
};
