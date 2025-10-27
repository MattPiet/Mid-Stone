#pragma once
#include "Camera.h"

class CameraController
{
public:
    explicit CameraController(Camera* cam) : camera(cam) {}

    void SetCamera(Camera* cam) { camera = cam; }


    void OnMouseButtonPressed(int button, Vec2 pixel);
    void OnMouseButtonReleased(int button);
    void OnMouseMoved(const Vec2& pixel) const;
    void OnMouseWheel(const float steps, const Vec2& pixel) const;
    /**
     * The code for the button
     * 2 equals middle button
     */
    int mouseButtonDrag = 2;

private:
    Camera* camera;
    bool isDragging = false;
    Vec2 dragStartScreen;
    Vec2 dragStartWorld;
    Vec2 dragStartCamera;
};
