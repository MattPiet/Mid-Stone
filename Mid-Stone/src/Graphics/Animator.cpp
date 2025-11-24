#include "Graphics/Animator.h"

void Animator::AddAnimationClip(const std::string& name, AnimationClip* clip)
{
    animationClips.emplace(name, clip);
    std::cout << "Added clip: " << name << std::endl;
}

void Animator::PlayAnimationClip(const std::string& name)
{
    if (const auto clip = GetAnimationClip(name))
    {
        std::cout << "Playing clip: " << name << '\n';
        currentClipName = name;
        currentClip = clip;
        currentClip->Reset();
        return;
    }
    currentClipName = "";
    currentClip = nullptr;
    std::cout << "Clip not found!" << '\n';
    throw std::runtime_error("Animation clip'" + name + "'not found.");
}

void Animator::Update(float deltaSeconds)
{
    if (currentClip)
    {
        currentClip->Update(deltaSeconds);
        //std::cout << "Current frame " << currentClip->getCurrentFrame() << std::endl;
        if (currentClip->GetIsFinished())
        {
            AnimationCallback();
        }
    }
}

void Animator::SetDefaultAnimation(const std::string name)
{
    if (animationClips.find(name) != animationClips.end())
    {
        defaultClipName = name;
        std::cout << "Default animation set to \"" << name << "\"\n";
    }
    else
    {
        std::cerr << "Error: Default animation \"" << name << "\" not found!\n";
    }
}

void Animator::AnimationCallback()
{
    if (currentClipName != defaultClipName)
    {
        if (currentClip && currentClip->GetIsFinished())
        {
            if (!defaultClipName.empty())
            {
                PlayAnimationClip(defaultClipName);
                std::cout << "playing default animation" << std::endl;
            }
        }
    }
}
