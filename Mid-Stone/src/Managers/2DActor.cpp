#include "Managers/2DActor.h"
#include <Utils/MemoryMonitor.h>

Actor2D::Actor2D()
{
    entity = nullptr;
    SpriteShader = nullptr;
    sprite_Mesh = nullptr;
    sprite_Renderer = nullptr;
    animator = nullptr;
}

Actor2D::~Actor2D()
{
    OnDestroy();
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
    SpriteShader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
    if (!SpriteShader->OnCreate())
    {
        std::cout << "Sprite Shader failed ... we have a problem\n";
        return false;
    }
    sprite_Mesh = new SpriteMesh();
    sprite_Mesh->OnCreate();

    sprite_Renderer = new SpriteRenderer();
    if (rows == NULL || columns == NULL)
        sprite_Renderer->loadImage(FileName);
    else
    {
        sprite_Renderer->loadImage(FileName, rows, columns);
        animator = new Animator();
    }

    entity = new Entity();
    entity->OnCreate(sprite_Renderer);

    return true;
}

void Actor2D::OnDestroy()
{
    delete entity;
    entity = nullptr;
    SpriteShader->OnDestroy();
    delete SpriteShader;
    SpriteShader = nullptr;
    sprite_Mesh->OnDestroy();
    delete sprite_Mesh;
    sprite_Mesh = nullptr;
    delete sprite_Renderer;
    sprite_Renderer = nullptr;
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
        animator->update(deltaTime);
    }
    /** Evaluate LifeSpan **/
    EvaluateLifeSpan(deltaTime);
}

void Actor2D::Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const
{
    if (draw_Hitbox)
    {
        entity->DrawHitBox(projectionMatrix, viewMatrix, sprite_Mesh);
    }
    if (sprite_Renderer->GetColumns() != NULL)
    {
        glUseProgram(SpriteShader->GetProgram());
        glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                           projectionMatrix);
        glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
        sprite_Renderer->renderSpriteSheet(SpriteShader, sprite_Mesh, entity->GetModelMatrix(),
                                           animator->getCurrentClip()->getCurrentFrame());
    }
    else
    {
        glUseProgram(SpriteShader->GetProgram());
        glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                           projectionMatrix);
        glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
        sprite_Renderer->renderSprite(SpriteShader, sprite_Mesh, entity->GetModelMatrix());
    }
}

void Actor2D::AddClip(const std::string& name, const AnimationClip* clip) const
{
    animator->addAnimationClip(name, const_cast<AnimationClip*>(clip));
}

void Actor2D::ReBuildAll(const char* FileName, int rows, int columns)
{
    if (rows != NULL && columns != NULL)
    {
        sprite_Renderer->loadImage(FileName, rows, columns);
        entity->CreateHitBox(sprite_Renderer, animator->getCurrentClip()->getCurrentFrame());
    }
    else
    {
        sprite_Renderer->loadImage(FileName);
        entity->CreateHitBox(sprite_Renderer);
    }
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
    if (VMath::mag(vel) > VERY_SMALL) {
        Vec3 dir = VMath::normalize(vel);
        Vec3 localForward(1.0f, 0.0f, -1.0f);  
        Vec3 currentForward = MATH::QMath::rotate(localForward, entity->GetOrientation());
        float dot = VMath::dot(currentForward, dir);
        float angle = acosf(std::clamp(dot, -1.0f, 1.0f)); // clamp to avoid NaNs
        Quaternion deltaRot = MATH::QMath::angleAxisRotation(angle * RADIANS_TO_DEGREES, Vec3(0.0f, 0.0f, 1.0f));
        Quaternion targetOri = deltaRot * entity->GetOrientation();
        Quaternion finalOri = MATH::QMath::slerp(entity->GetOrientation(), targetOri, deltaTime * 25.0f); // last number used as rotation speed
        entity->SetOrientation(finalOri);
    }
}
