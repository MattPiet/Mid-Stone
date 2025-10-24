#pragma once
#include <functional>
#include <Matrix.h>
#include <Quaternion.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Graphics/Shader.h>
#include <Graphics/SpriteMesh.h>
#include <Graphics/SpriteRenderer.h>


class Entity
{
private:


    MATH::Vec3 scale;
    MATH::Vec3 position;

    char hitBoxType; // 'c' for circle, 'q' for quad, etc.
    MATH::Vec3 hitbox;
	MATH::Vec4 hitboxColor{ 0.0f, 1.0f, 0.0f, 1.0f }; // RGBA

	Shader* shader;

    using ExpiredCallback = std::function<void(Entity&)>;
    /**
     * Callback for a function that will be executed once this entity expires
     */
    ExpiredCallback onExpired;

    /**
     * Life span for the entity, if it's 0, the object is immortal, if not, it will be the time in
     * seconds that the entity will live before destructing itself.
     */
    float lifeSpanSeconds{ 0.0f };

    /**
     * Counter of seconds to calculate the lifespan of this entity.
     */
    float currentLifeTimeSeconds{ 0.0f };
    bool hasFiredExpiredHooks{ false };

protected:

    MATH::Quaternion orientation;

    virtual void OnExpired()
    {
        std::cout << "Entity expired" << std::endl;
    }

public:
    Entity() = default;
    Entity(const MATH::Vec3& position, const MATH::Vec3& scale, const char& hitBoxType);

    void OnCreate(SpriteRenderer* renderer);
	void OnDestroy();

    virtual ~Entity()
    {
        std::cout << "Entity destroyed" << std::endl;
    };

    void SetLifeSpan(float lifeSpanSeconds);
    [[nodiscard]] float GetLifeSpan() const { return lifeSpanSeconds; }
    [[nodiscard]] float GetCurrentLifeTime() const { return currentLifeTimeSeconds; }
    void SetExpiredCallback(ExpiredCallback callback) { onExpired = std::move(callback); }

    char GetHitBoxType() const { return hitBoxType; }
    

    /**
     * Checks whether the current lifetime surpasses the expected lifespan, as long as the lifespan is > 0
     * @return boolean indicating whether it is expired or not
     */
    [[nodiscard]] bool IsExpired() const;
    [[nodiscard]] bool IsImmortal() const { return lifeSpanSeconds <= 0.0f; }


    [[nodiscard]] MATH::Vec3 GetPosition() const { return position; }
    [[nodiscard]] MATH::Vec3 GetScale() const { return scale; }

    [[nodiscard]] MATH::Matrix4 GetModelMatrix() const;

    void Update(float deltaTime);

	void DrawHitBox(MATH::Matrix4 projectionMatrix, SpriteMesh* mesh);

    void SetHitboxColor(const MATH::Vec4& color) {
        hitboxColor = color;
    }

    MATH::Vec3 GetHitbox() const {
        return hitbox;
	}

    void AdjustHitboxSize(const MATH::Vec3& adjustment) {
        hitbox = hitbox + adjustment;
	}
};
