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
   // The playback mode of the animation clip (determines if it loops or plays once etc.)  
   PlayMode playMode;
  // Duration of each frame in seconds  
  float frameDuration;  
  // The total number of frames in the clip  
  int totalFrames;
  // The current frame in the clip  
  int currentFrame;  
  
  int rows;
  int columns;
  float secondsSinceLastUpdate;
  bool isPlayingForward;
  bool isFinished;


public:  
  // Default constructor which sets the frame duration to 0.1 with a default playmode to loop  
  AnimationClip() :  
      playMode{ PlayMode::LOOP }, 
      frameDuration{ 0.1f },  
   rows{ 1 },
   columns{ 1 },
      totalFrames { 1 }, 
      currentFrame{ 1 },
   secondsSinceLastUpdate{ 0.0f },
   isPlayingForward{ true },
   isFinished{ false }
  
  {}

  //dynamic constructor 
  AnimationClip(PlayMode playMode_ = PlayMode::ONCE, float frameDuration_ = 0.1f, int rows_ = 1, int columns_ = 1) : 
      playMode{ playMode_ },
      frameDuration{ frameDuration_ },
      rows{ rows_ },
      columns{ columns_ },
      totalFrames{ columns_ * rows_ }, 
      currentFrame{ 1 },
      secondsSinceLastUpdate{ 0.0f },
      isPlayingForward{ true },
      isFinished{ false }
  {}

  // Basic getter to retrieve the frame duration  
  float getFrameDuration() const { return frameDuration; }  

  // Basic getter to retrieve the playback mode of the animation  
  PlayMode getPlayMode() const { return playMode; }  

  // Getter for total frames
  int getTotalFrames() const { return totalFrames; }

  // Getter for current frame
  int getCurrentFrame() const { return currentFrame; }

  // Getter for seconds since last update
  float getSecondsSinceLastUpdate() const { return secondsSinceLastUpdate; }

  // Getter for playback direction
  bool getIsPlayingForward() const { return isPlayingForward; }

  // Getter for finished state
  bool getIsFinished() const { return isFinished; }

  // Resets the animation clip to the first frame and initial state
  void reset() { 
  currentFrame = 0; 
  secondsSinceLastUpdate = 0.0f;
  isPlayingForward = true;
  isFinished = false;
  }  

  // 
  void update(float deltaSeconds);


  void render();
};
