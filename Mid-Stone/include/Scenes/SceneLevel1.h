#pragma once
#include "Scene.h"

class SceneLevel1 : public Scene
{
private:
public:
    explicit SceneLevel1();
    virtual ~SceneLevel1();

    /** Scene Class Methods **/
    void Render() const override;
    void Update(const float deltaTime) override;
    void HandleEvents(const SDL_Event& sdlEvent) override;
    bool OnCreate() override;
    void RenderGUI() override;
    void OnDestroy() override;
    
};
