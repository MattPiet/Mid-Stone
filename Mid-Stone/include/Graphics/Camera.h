#pragma once
#include <Matrix.h>
#include <MMath.h>
#include <Vector.h>

struct CameraBounds
{
    float minX, minY, maxX, maxY;
};

class Camera
{
private:
    /**
     * World Dimension to clamp to
     */
    
    [[nodiscard]] float HalfWidthWorldForZoom(float z) const;
    [[nodiscard]] float HalfHeightWorldForZoom(float z) const;
public:
    CameraBounds clampWorld{-200.0f, -200.0f, 200.0f, 200.0f};
    float cameraZoom = 1.0f;
    float minimumZoom = 0.8f;
    float maximumZoom = 3.0f;
    Vec2 cameraPosition;

    float viewportWidth = 1920;
    float viewportHeight = 1080;

    bool clampToWorld = true;
    /**
     *  pixelsPerUnit lets you define how many pixels represent 1 world unit at zoom = 1
     */
    float pixelsPerUnit = 10.0f;
    
    /**
     * Clamps the camera if clampToWorld is on
     * @param clampZoom 
     */
    void ClampIfNeeded(float clampZoom);
    
    [[nodiscard]] Matrix4 GetProjectionMatrix() const;
    [[nodiscard]] Matrix4 GetViewMatrix() const;

    /**
     * 
     * @param screenPixel Converts from screen pixel to world pixels
     * @return Vec2 with the world coordinates
     */
    [[nodiscard]] Vec2 ScreenToWorld(const Vec2& screenPixel) const;

    /**
     * Zooms the camera
     * @param zoomFactor 
     * @param worldAnchor The point to anchor the zoom, as in, which is is the point that will be zoomed in
     */
    void ZoomAtWorldPoint(float zoomFactor, const Vec2& worldAnchor);

    void SetViewportSize(const int width, const int height);


    /**
     * Helper function to move the camera by code if needed
     * @param worldDelta 
     */
    void PanByWorldDelta(const Vec2& worldDelta);

    
};
