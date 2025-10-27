#include "Graphics/CameraController.h"

void CameraController::OnMouseButtonPressed(int button, Vec2 pixel)
{
    if (button == mouseButtonDrag)
    {
        isDragging = true;
        dragStartScreen = pixel;
        dragStartWorld = camera->ScreenToWorld(pixel);
        dragStartCamera = camera->cameraPosition;
    }
}

void CameraController::OnMouseButtonReleased(int button)
{ if (button == mouseButtonDrag) isDragging = false; }

void CameraController::OnMouseMoved(const Vec2& pixel) const
{
    if (!isDragging) return;

    const Vec2 curWorld = camera->ScreenToWorld(pixel);
    const Vec2 delta = dragStartWorld - curWorld;
    
    camera->cameraPosition += delta;
    camera->ClampIfNeeded(camera->minimumZoom);
}

void CameraController::OnMouseWheel(const float steps, const Vec2& pixel) const
{
    const float factor = (steps > 0) ? 1.1f : 0.9f;
    camera->ZoomAtWorldPoint(factor, camera->ScreenToWorld(pixel));
}
