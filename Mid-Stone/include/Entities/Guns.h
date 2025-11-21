#pragma once
#include <iostream>
#include <functional>
#include <Matrix.h>
#include <Quaternion.h>
#include <Managers/PlayerController.h>
#include <Managers/2DActor.h>
#include <SDL_mixer.h>
#include <memory> 
using namespace MATH; 
class Actor2D;
class PlayerController;
class Guns {
	MIX_Mixer* mixer;
public:
    enum class GunType {
        PISTOL,
        SHOTGUN,
        RIFLE,
        // ... other gun types
    };
    bool funnyNoises = false;
    GunType currentGunType;
    Guns();
	~Guns();
    void setGunType(GunType type);
    std::vector<std::unique_ptr<Actor2D>> shoot(PlayerController* controller, std::vector<std::unique_ptr<Actor2D>>& impacts);
	void setMixer(MIX_Mixer* mixer_) { mixer = mixer_; }
	void playShootSound();
};

