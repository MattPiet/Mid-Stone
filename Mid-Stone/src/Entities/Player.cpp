#include "../../include/Entities/Player.h"

#include <MMath.h>
#include <QMath.h>

Player::Player(const MATH::Vec3& position, const MATH::Vec3& scale, const char& hitBoxType) : Entity(position, scale, hitBoxType)
{
}

void Player::MoveAim(float angleInDegrees)
{
    MATH::Quaternion rotation = MATH::QMath::angleAxisRotation(angleInDegrees, MATH::Vec3(0.0f, 0.0f, 1.0f));
    this->crossHairsDirection = rotation * this->crossHairsDirection;
}

MATH::Matrix4 Player::GetAimModelMatrix() const
{
    const MATH::Matrix4 initial_translation = MMath::translate(Vec3(this->crossHairsOffset, 0.0f, 0.0f));
    const MATH::Matrix4 translation = MMath::translate(this->GetPosition());
    const MATH::Matrix4 rotation = MMath::toMatrix4(crossHairsDirection);
    const MATH::Matrix4 modelScale = MMath::scale(this->crossHairsScale);
    return translation * rotation *  initial_translation * modelScale ;
}

std::unique_ptr<Entity> Player::Shoot() const
{
    const Vec3 localForwardOffset(crossHairsOffset, 0.0f, 0.0f);
    const MATH::Matrix4 TRL = MMath::translate(GetPosition()) *
                              MMath::toMatrix4(crossHairsDirection) *
                              MMath::translate(localForwardOffset);
    const Vec4 worldPoint = TRL * Vec4(0,0,0,1);
    const Vec3 worldMuzzlePosition(worldPoint.x, worldPoint.y, worldPoint.z);
    
    auto bullet = std::make_unique<Entity>(worldMuzzlePosition, Vec3{1.0f, 1.0f, 1.0f}, 'q');
   
    bullet->SetLifeSpan(2.0f);
    return bullet;
}
