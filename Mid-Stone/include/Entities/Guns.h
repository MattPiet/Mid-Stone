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
    enum class Gun_type {
        pistol,
        shotgun,
        rifle,
        // ... other gun types
    };
    bool funnyNoises = false;
    Gun_type currentGunType;
    Guns();
	~Guns();
    void SetGunType(Gun_type type);
    std::vector<std::unique_ptr<Actor2D>> Shoot(PlayerController* controller, std::vector<std::unique_ptr<Actor2D>>& impacts);
	void SetMixer(MIX_Mixer* mixer_) { mixer = mixer_; }
	void PlayShootSound();
};

