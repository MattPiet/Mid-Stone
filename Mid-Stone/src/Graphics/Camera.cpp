#include "Graphics/Camera.h"


float Camera::HalfWidthWorldForZoom(const float z) const
{
    return viewportWidth * 0.5f / (pixelsPerUnit * z);
}

float Camera::HalfHeightWorldForZoom(const float z) const
{
    return viewportHeight * 0.5f / (pixelsPerUnit * z);
}

void Camera::ClampIfNeeded(const float clampZoom)
{
    if (!clampToWorld) return;

    const float halfWidth = HalfWidthWorldForZoom(clampZoom);
    const float halfHeight = HalfHeightWorldForZoom(clampZoom);

    const float minX = clampWorld.minX + halfWidth;
    const float maxX = clampWorld.maxX - halfWidth;
    const float minY = clampWorld.minY + halfHeight;
    const float maxY = clampWorld.maxY - halfHeight;

    cameraPosition.x = std::clamp(cameraPosition.x, minX, maxX);
    cameraPosition.y = std::clamp(cameraPosition.y, minY, maxY);
}

Matrix4 Camera::GetProjectionMatrix() const
{
    const float halfWidth = viewportWidth * 0.5f / pixelsPerUnit / cameraZoom;
    const float halfHeight = viewportHeight * 0.5f / pixelsPerUnit / cameraZoom;
    return MMath::orthographic(-halfWidth, +halfWidth, -halfHeight, +halfHeight, -1.0f, 1.0f);
}

Matrix4 Camera::GetViewMatrix() const
{
    return MMath::translate(Vec3(-cameraPosition.x, -cameraPosition.y, 0.0f));
}

Vec2 Camera::ScreenToWorld(const Vec2& screenPixel) const
{
    // NDC
    const float x = 2.0f * screenPixel.x / static_cast<float>(viewportWidth) - 1.0f;
    const float y = 1.0f - 2.0f * screenPixel.y / static_cast<float>(viewportHeight);
    const Vec4 ndc(x, y, 0.0f, 1.0f);

    const Matrix4 projection = GetProjectionMatrix();
    const Matrix4 view = GetViewMatrix();
    const Matrix4 inversePv = MMath::inverse(projection * view);

    Vec4 world = inversePv * ndc;
    return {world.x, world.y};
}

void Camera::SetViewportSize(const int width, const int height)
{
    viewportWidth = static_cast<float>(width);
    viewportHeight = static_cast<float>(height);
}

void Camera::ZoomAtWorldPoint(float zoomFactor, const Vec2& worldAnchor)
{
    const float previousZoom = cameraZoom;
    cameraZoom = std::clamp(cameraZoom * zoomFactor, minimumZoom, maximumZoom);
    const float scale = 1.0f / previousZoom - 1.0f / cameraZoom;
    cameraPosition += (worldAnchor - cameraPosition) * scale;

    ClampIfNeeded(minimumZoom);
}

void Camera::PanByWorldDelta(const Vec2& worldDelta)
{
    cameraPosition += worldDelta;
    ClampIfNeeded(minimumZoom);
}
