#include "Managers/2DActor.h"
#include <Utils/MemoryMonitor.h>

Actor2D::Actor2D()
{
    entity = nullptr;
    spriteShader = nullptr;
    spriteMesh = nullptr;
    spriteRenderer = nullptr;
    animator = nullptr;
}

Actor2D::~Actor2D()
{
    Actor2D::OnDestroy();
}


/**
 * @brief Creates and initializes the necessary components for the Actor2D object.
 *
 * This function sets up shaders, sprite meshes, sprite renderers, and an animator if needed.
 * It also handles the initialization of an associated entity.
 *
 * @param FileName The file path of the sprite image to be loaded.
 * @param rows The number of rows in the sprite sheet (for animated sprites). If NULL, it loads a single image.
 * @param columns The number of columns in the sprite sheet (for animated sprites). If NULL, it loads a single image.
 *
 * @return True if all components were successfully created and initialized. False if any step failed.
 *
 * @details
 * - Creates and initializes `Shader` objects for sprite and hitbox rendering. Logs errors if initialization fails.
 * - Creates the `SpriteMesh` object.
 * - Initializes the `SpriteRenderer` to load the sprite image. If rows and columns are specified, the renderer is
 *   set up for animation, and an `Animator` is created.
 * - Creates the associated `Entity` object and links the `SpriteRenderer` to it.
 */
bool Actor2D::OnCreate(const char* FileName, int rows, int columns)
{
    spriteShader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
    if (!spriteShader->OnCreate())
    {
        std::cout << "Sprite Shader failed ... we have a problem\n";
        return false;
    }
    spriteMesh = new SpriteMesh();
    spriteMesh->OnCreate();

    spriteRenderer = new SpriteRenderer();
    if (rows == NULL || columns == NULL)
        spriteRenderer->loadImage(FileName);
    else
    {
        spriteRenderer->loadImage(FileName, rows, columns);
        animator = new Animator();
    }

    entity = new Entity();
    entity->OnCreate(spriteRenderer);

    return true;
}

void Actor2D::OnDestroy()
{
    delete entity;
    entity = nullptr;
    spriteShader->OnDestroy();
    delete spriteShader;
    spriteShader = nullptr;
    spriteMesh->OnDestroy();
    delete spriteMesh;
    spriteMesh = nullptr;
    delete spriteRenderer;
    spriteRenderer = nullptr;
    delete animator;
    animator = nullptr;
}


void Actor2D::Update(const float deltaTime)
{
    /** Updating Entities **/
    entity->Update(deltaTime);
    /** Update Animators if present **/
    if (animator != nullptr)
    {
        animator->Update(deltaTime);
    }
    /** Evaluate LifeSpan **/
    EvaluateLifeSpan(deltaTime);
}

void Actor2D::Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const
{
    if (drawHitbox)
    {
        entity->DrawHitBox(projectionMatrix, viewMatrix, spriteMesh);
    }
    if (spriteRenderer->GetColumns() != NULL)
    {
        glUseProgram(spriteShader->GetProgram());
        glUniformMatrix4fv(static_cast<GLint>(spriteShader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                           projectionMatrix);
        glUniformMatrix4fv(static_cast<GLint>(spriteShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
        spriteRenderer->renderSpriteSheet(spriteShader, spriteMesh, entity->GetModelMatrix(),
                                           animator->GetCurrentClip()->GetCurrentFrame());
    }
    else
    {
        glUseProgram(spriteShader->GetProgram());
        glUniformMatrix4fv(static_cast<GLint>(spriteShader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                           projectionMatrix);
        glUniformMatrix4fv(static_cast<GLint>(spriteShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
        spriteRenderer->renderSprite(spriteShader, spriteMesh, entity->GetModelMatrix());
    }
}

void Actor2D::AddClip(const std::string& name, const AnimationClip* clip) const
{
    animator->AddAnimationClip(name, const_cast<AnimationClip*>(clip));
}

void Actor2D::ReBuildAll(const char* FileName, int rows, int columns)
{
    if (rows != NULL && columns != NULL)
    {
        spriteRenderer->loadImage(FileName, rows, columns);
        entity->CreateHitBox(spriteRenderer, animator->GetCurrentClip()->GetCurrentFrame());
    }
    else
    {
        spriteRenderer->loadImage(FileName);
        entity->CreateHitBox(spriteRenderer);
    }
}

void Actor2D::TakeDamage(const int damage)
{
    // Actors without health don't count
    if (health == NULL) return;
    health -= damage;
    if (health <= 0)
    {
        Perish();
    }
}

void Actor2D::Perish()
{
    // If the entity has a death animation, play it
    if (animator != nullptr)
    {
        std::cout << "Actor2D Perished With Animation" << std::endl;
        if (auto const deathClip = animator->GetAnimationClip("Death"))
        {
            /** Disable collition response **/
            onCollisionCallback = nullptr;
            animator->PlayAnimationClip("Death");
            const float clipDuration = deathClip->GetAnimationDuration();
            std::cout << "Clip Duration Death: " << clipDuration << std::endl;
            ConfigureLifeSpan(clipDuration);
            return;
        }
    }
    ConfigureLifeSpan(0.01f);
}

void Actor2D::ConfigureLifeSpan(const float newLifeSpanSeconds)
{
    lifeSpanSeconds = newLifeSpanSeconds;
    elapsedLifeTimeSeconds = 0.0f;
    hasTriggeredExpiredHook = false;
}

void Actor2D::EvaluateLifeSpan(float deltaTime)
{
    if (lifeSpanSeconds > 0.0f)
    {
        elapsedLifeTimeSeconds += std::max(0.0f, deltaTime);
        if (!hasTriggeredExpiredHook && elapsedLifeTimeSeconds >= lifeSpanSeconds)
        {
            hasTriggeredExpiredHook = true;
            OnExpire();
            if (onExpireCallback) { onExpireCallback(*this); }
        }
    }
}


/** Adjusts Actors Rotation To Face Where It's Moving **/
void Actor2D::FaceVelocity(float deltaTime)
{
    Vec3 vel = entity->GetVelocity();
    if (VMath::mag(vel) > VERY_SMALL)
    {
        Vec3 dir = VMath::normalize(vel);
        Vec3 localForward(1.0f, 0.0f, -1.0f);
        Vec3 currentForward = MATH::QMath::rotate(localForward, entity->GetOrientation());
        float dot = VMath::dot(currentForward, dir);
        float angle = acosf(std::clamp(dot, -1.0f, 1.0f)); // clamp to avoid NaNs
        Quaternion deltaRot = MATH::QMath::angleAxisRotation(angle * RADIANS_TO_DEGREES, Vec3(0.0f, 0.0f, 1.0f));
        Quaternion targetOri = deltaRot * entity->GetOrientation();
        Quaternion finalOri = MATH::QMath::slerp(entity->GetOrientation(), targetOri, deltaTime * 25.0f);
        // last number used as rotation speed
        entity->SetOrientation(finalOri);
    }
}
