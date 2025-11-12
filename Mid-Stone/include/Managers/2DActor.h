#pragma once
#include <Managers/Actor.h>

class Entity;
class Shader;
class SpriteMesh;
class SpriteRenderer;
class AnimationClip;
class Animator;

class Actor2D : public Actor
{
private:
    /** --------- LifeSpan management --------- **/
    using ExpiredCallback = std::function<void(Actor2D&)>;
    /**
     * Callback for a function that will be executed once this entity expires
     */
    ExpiredCallback onExpireCallback;
    /**
     * Life span for the entity, if it's 0, the object is immortal, if not, it will be the time in
     * seconds that the entity will live before destructing itself.
     */
    float lifeSpanSeconds{0.0f};
    /**
     * Counter of seconds to calculate the lifespan of this entity.
     */
    float elapsedLifeTimeSeconds{0.0f};
    bool hasTriggeredExpiredHook{false};

    Entity* entity;
    Shader* SpriteShader;
    SpriteMesh* sprite_Mesh;
    SpriteRenderer* sprite_Renderer;
    Animator* animator;

public:
    Actor2D();
    virtual ~Actor2D();
    
    /** Lifecycle Functions **/
    bool OnCreate(const char* FileName, int rows = NULL, int columns = NULL);

    virtual void OnExpire()
    {
        std::cout << "Entity expired" << std::endl;
    }

    /**
     * Updates the state of the Actor2D instance for the current frame.
     * This method processes animations, updates related entities, and evaluates the entity's lifespan.
     *
     * @param deltaTime The time elapsed, in seconds, since the last frame. Used to calculate updates to the actor's state.
     */
    void Update(const float deltaTime) override;

    bool draw_Hitbox = false;
	bool isStatic = false;
    
    virtual void OnDestroy() override;
    
    virtual void Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const override;

    void AddClip(const std::string& name, const AnimationClip* clip) const;

    void ReBuildAll(const char* FileName, int rows = NULL, int columns = NULL);

    Entity* GetEntity() const { return entity; }
    SpriteMesh* GetMesh() const { return sprite_Mesh; }
    SpriteRenderer* GetRenderer() { return sprite_Renderer; }
    Animator* GetAnimator() const { return animator; }


    /** --------- LifeSpan management --------- **/
    
    /**
     * Checks if the entity has expired and triggered its expiration hook.
     *
     * @return true if the entity has expired; otherwise, false.
     */
    [[nodiscard]] bool HasExpired() const
    {
        return hasTriggeredExpiredHook;
    }

    /**
     * Configures the lifespan of the Actor2D instance.
     * If the specified lifespan is greater than 0, the actor will automatically expire after the given duration in seconds.
     * A lifespan of 0 indicates that the actor is immortal and will not expire.
     * The method also resets the elapsed lifetime and ensures the expiration hook has not yet been triggered.
     *
     * @param newLifeSpanSeconds The new lifespan for the actor in seconds. A value of 0 makes the actor immortal.
     */
    void ConfigureLifeSpan(float newLifeSpanSeconds);
    /**
     * Evaluates the lifespan of the entity based on the elapsed time,
     * updating its status and triggering the expiration logic if necessary.
     *
     * @param deltaTime The time in seconds since the last update, used to increment the elapsed lifetime.
     */
    void EvaluateLifeSpan(float deltaTime);

    /**
     * Registers a callback function to be executed when the entity expires.
     *
     * @param callback A function object that takes a reference to an Actor2D instance and is called upon expiration.
     */
    void RegisterExpiredCallback(ExpiredCallback callback) { onExpireCallback = std::move(callback); }
    /**
     * Determines whether the entity is immortal based on its lifespan.
     * An entity is considered immortal if its lifespan is less than or equal to zero.
     * @return true if the entity is immortal, false otherwise
     */
    [[nodiscard]] bool IsImmortal() const { return lifeSpanSeconds <= 0.0f; }
    /**
     * Retrieves the lifespan of the entity in seconds.
     * A value of 0 indicates the entity is immortal.
     *
     * @return The lifespan of the entity in seconds.
     */
    [[nodiscard]] float GetLifeSpan() const { return lifeSpanSeconds; }
    /**
     * Retrieves the amount of time, in seconds, that has elapsed since the entity's lifespan tracking began.
     * @return The elapsed lifespan in seconds.
     */
    [[nodiscard]] float GetElapsedLifeTime() const { return elapsedLifeTimeSeconds; }



};
