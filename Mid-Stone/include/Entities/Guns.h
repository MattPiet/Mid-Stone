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
    const char* bulletsSprite;
    float pistolExpiration, rifleExpiration, shotgunExpiration;
    Vec3 scale;
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
    void setBulletSprite(const char* sprite) { bulletsSprite = sprite; }
    void setPistolExpiration(float time) { pistolExpiration = time; }
    void setShotgunExpiration(float time) { shotgunExpiration = time; }
    void setRifleExpiration(float time) { rifleExpiration = time; }
    void setSpriteScale(float size) { scale *= size; }
};


