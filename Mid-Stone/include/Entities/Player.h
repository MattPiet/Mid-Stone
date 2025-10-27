#pragma once
#include <iostream>
#include <ostream>

#include "Entity.h"

class Player : public Entity
{
private:
    /**
     * The angle in which the bullet will be shot.
     */
    MATH::Quaternion crossHairsDirection;
    MATH::Vec3 crossHairsScale = {0.5f, 0.5f, 0.5f};
    float crossHairsOffset = 15.0f;

protected:
    void OnExpired() override
    {
        std::cout << "Player expired" << std::endl;
    }

public:
    Player() = default;

    Player(const MATH::Vec3& position, const MATH::Vec3& scale);
    Player(const MATH::Vec3& position, const MATH::Vec3& scale, const Hit_box_type& hitBoxType);
    ~Player() override
    {
        std::cout << "Player destroyed" << std::endl;
    };

    void MoveAim(float angleInDegrees);
    [[nodiscard]] MATH::Matrix4 GetAimModelMatrix() const;

    /** Shoots a bullet, returns the created bullet for the main scene to handle **/
    std::unique_ptr<Entity> Shoot() const;
};
