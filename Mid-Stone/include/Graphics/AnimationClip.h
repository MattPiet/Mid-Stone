#pragma once  
class AnimationClip {  
public:  
   enum class PlayMode {  
       ONCE,  
       LOOP,  
       PINGPONG,  
       REVERSE  
   };  
private:  
   // Duration of each frame in seconds  
   float frameDuration;  
   // The playback mode of the animation clip (determines if it loops or plays once etc.)  
   PlayMode playMode;  
   // The current frame in the clip  
   int currentFrame;  
   // The total number of frames in the clip  
   int totalFrames;  
public:  
   // Default constructor which sets the frame duration to 0.1 with a default playmode to loop  
   AnimationClip() : frameDuration{ 0.1f }, playMode{ PlayMode::LOOP }, currentFrame{ 0 }, totalFrames{ 0 } {}  
 
   // Dynamic constructor to fill in values  
   AnimationClip(float frameDuration_, PlayMode playMode_)  
       : frameDuration{ frameDuration_ }, playMode{ playMode_ }, currentFrame{ 0 }, totalFrames{ 0 } {}  
 
   // Basic getter to retrieve the frame duration  
   float getFrameDuration() const { return frameDuration; }  
 
   // Basic getter to retrieve the playback mode of the animation  
   PlayMode getPlayMode() const { return playMode; }  
 
   void resetFrame() { currentFrame = 0; }  
};
