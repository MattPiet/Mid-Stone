#include <QMath.h>
#include <Managers/PlayerController.h>
#include <Utils/MemoryMonitor.h>

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
    crosshairsShader->OnDestroy();
    delete crosshairsShader;
	crosshairsShader = nullptr;
    crosshairsMesh->OnDestroy();
    delete crosshairsMesh;
	crosshairsMesh = nullptr;
	delete crosshairsRenderer;
}

bool PlayerController::OnCreate(const char* crossHairsFilename)
{
    crosshairsShader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
    if (!crosshairsShader->OnCreate())
    {
        std::cout << "Sprite Shader failed ... we have a problem\n";
        return false;
    }
    crosshairsMesh = new SpriteMesh();
    crosshairsMesh->OnCreate();

    crosshairsRenderer = new SpriteRenderer();
    crosshairsRenderer->loadImage(crossHairsFilename);
    return true;
}

void PlayerController::MoveAim(float angleInDegrees)
{
    MATH::Quaternion rotation = MATH::QMath::angleAxisRotation(angleInDegrees, MATH::Vec3(0.0f, 0.0f, 1.0f));
    this->crossHairsDirection = rotation * this->crossHairsDirection;
}

MATH::Matrix4 PlayerController::GetAimModelMatrix() const
{
    const MATH::Matrix4 initial_translation = MMath::translate(Vec3(this->crossHairsOffset, 0.0f, 0.0f));
    const MATH::Matrix4 translation = MMath::translate(this->possessedActor->getEntity()->GetPosition());
    const MATH::Matrix4 rotation = MMath::toMatrix4(crossHairsDirection);
    const MATH::Matrix4 modelScale = MMath::scale(this->crossHairsScale);
    return translation * rotation * initial_translation * modelScale;
}

void PlayerController::RenderCrossHairs(Matrix4 viewMatrix, Matrix4 projectionMatrix) const
{
    glUseProgram(crosshairsShader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(crosshairsShader->GetUniformID("projectionMatrix")), 1, GL_FALSE,
                       projectionMatrix);
    glUniformMatrix4fv(static_cast<GLint>(crosshairsShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
    crosshairsRenderer->renderSprite(crosshairsShader, crosshairsMesh, this->GetAimModelMatrix());
}
