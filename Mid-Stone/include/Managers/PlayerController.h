#pragma once
#include <Quaternion.h>

#include "2DActor.h"

class PlayerController
{
private:
    Actor2D* possessedActor;

    /** Crosshairs Features **/
    MATH::Quaternion crossHairsDirection;
    MATH::Vec3 crossHairsScale = {0.5f, 0.5f, 0.5f};
    float crossHairsOffset = 15.0f;
    Shader* crosshairsShader;
    SpriteMesh* crosshairsMesh;
    SpriteRenderer* crosshairsRenderer;

public:
    PlayerController();
    ~PlayerController();

    bool OnCreate(const char* crossHairsFilename);


    [[nodiscard]] Actor2D* GetPossessedActor() const
    {
        return possessedActor;
    }

    void SetPossessedActor(Actor2D* newPossessedActor)
    {
        this->possessedActor = newPossessedActor;
    }
    
    void MoveAim(float angleInDegrees);
    [[nodiscard]] MATH::Matrix4 GetAimModelMatrix() const;
    void RenderCrossHairs(Matrix4 viewMatrix, Matrix4 projectionMatrix) const;
    Vec3 GetCrossHairsPosition() const;
    
};
