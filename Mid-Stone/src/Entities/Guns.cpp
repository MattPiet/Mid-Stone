#include <Entities/Guns.h>
#include <Utils/MemoryMonitor.h>

Guns::Guns()
{
    currentGunType = Guns::Gun_type::pistol;
    mixer = nullptr;
}

Guns::~Guns()
{
}

void Guns::SetGunType(Gun_type type)
{
    switch (type)
    {
    case Guns::Gun_type::pistol:
        std::cout << "Gun type set to PISTOL" << '\n';
        currentGunType = Guns::Gun_type::pistol;
        break;
    case Guns::Gun_type::shotgun:
        std::cout << "Gun type set to SHOTGUN" << '\n';
        currentGunType = Guns::Gun_type::shotgun;
        break;
    case Guns::Gun_type::rifle:
        std::cout << "Gun type set to RIFLE" << '\n';
        currentGunType = Guns::Gun_type::rifle;
        break;
    default:
        std::cout << "Unknown gun type" << '\n';
        break;
    }
}

std::vector<std::unique_ptr<Actor2D>> Guns::Shoot(PlayerController* controller,
                                                  std::vector<std::unique_ptr<Actor2D>>& impacts)
{
    std::vector<std::unique_ptr<Actor2D>> bullets;

    const Vec3 pos = controller->GetCrossHairsPosition();
    const auto quat = controller->GetCrossHairsRotation();
    const Vec3 forward(1.0f, 0.0f, 0.0f);

    switch (currentGunType)
    {
    case Guns::Gun_type::pistol:
        {
            const Vec3 velocity = quat * forward * QMath::inverse(quat) * 100.0f;

            auto bullet = std::make_unique<Actor2D>();
            bullet->OnCreate("sprites/fatty_clicked.png");
            bullet->GetEntity()->SetPosition(pos);
            bullet->GetEntity()->AdjustOrientation(quat);
            bullet->GetEntity()->SetVelocity(velocity);
            bullet->SetTag(Actor_tags::bullets);
            bullet->ConfigureLifeSpan(5.0f);

            bullet->RegisterExpiredCallback([&impacts](const Actor2D& actor)
            {
                auto impact = std::make_unique<Actor2D>();
                impact->OnCreate("sprites/impact.png", 2, 4);
                impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
                impact->ConfigureLifeSpan(0.27f);
                const auto clip = new AnimationClip(AnimationClip::Play_mode::once, 0.03f, 2, 4, 0, 7);
                impact->GetAnimator()->AddDefaultAnimationClip("Idle", clip);
                impact->GetAnimator()->PlayAnimationClip("Idle");
                impacts.push_back(std::move(impact));
            });

            if (funnyNoises) { PlayShootSound(); }
            bullets.push_back(std::move(bullet));
            break;
        }

    case Guns::Gun_type::shotgun:
        {
            for (int i = 0; i < 5; ++i)
            {
                Vec3 spreadPos = pos + Vec3(static_cast<float>(rand() % 21 - 10), static_cast<float>(rand() % 21 - 10),
                                            0.0f);
                const Vec3 velocity = quat * forward * QMath::inverse(quat) * 100.0f;
           
                auto bullet = std::make_unique<Actor2D>();
                bullet->OnCreate("sprites/fatty_clicked.png");
                bullet->GetEntity()->SetPosition(spreadPos);
                bullet->GetEntity()->AdjustOrientation(quat);
                bullet->GetEntity()->SetVelocity(velocity);
                bullet->SetTag(Actor_tags::bullets);
                bullet->ConfigureLifeSpan(5.0f);

                bullet->RegisterExpiredCallback([&impacts](const Actor2D& actor)
                {
                    auto impact = std::make_unique<Actor2D>();
                    impact->OnCreate("sprites/impact.png", 2, 4);
                    impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
                    impact->ConfigureLifeSpan(0.27f);
                    const auto clip = new AnimationClip(AnimationClip::Play_mode::once, 0.03f, 2, 4, 0, 7);
                    impact->GetAnimator()->AddDefaultAnimationClip("Idle", clip);
                    impact->GetAnimator()->PlayAnimationClip("Idle");
                    impacts.push_back(std::move(impact));
                });
                if (funnyNoises) { PlayShootSound(); }
                bullets.push_back(std::move(bullet));
            }
            break;
        }

    case Guns::Gun_type::rifle:
        {
            // Rifle is a faster single bullet with longer lifespan and higher speed
            const Vec3 velocity = quat * forward * QMath::inverse(quat) * 200.0f; // faster bullet

            auto bullet = std::make_unique<Actor2D>();
            bullet->OnCreate("sprites/fatty_clicked.png");
            bullet->GetEntity()->SetPosition(pos);
            bullet->GetEntity()->AdjustOrientation(quat);
            bullet->GetEntity()->SetVelocity(velocity);
            bullet->SetTag(Actor_tags::bullets);
            bullet->ConfigureLifeSpan(8.0f);

            bullet->RegisterExpiredCallback([&impacts](const Actor2D& actor)
            {
                auto impact = std::make_unique<Actor2D>();
                impact->OnCreate("sprites/impact.png", 2, 4);
                impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
                impact->ConfigureLifeSpan(0.27f);
                const auto clip = new AnimationClip(AnimationClip::Play_mode::once, 0.03f, 2, 4, 0, 7);
                impact->GetAnimator()->AddDefaultAnimationClip("Idle", clip);
                impact->GetAnimator()->PlayAnimationClip("Idle");
                impacts.push_back(std::move(impact));
            });
            if (funnyNoises) { PlayShootSound(); }
            bullets.push_back(std::move(bullet));
            break;
        }

    default:
        break;
    }

    return bullets;
}

void Guns::PlayShootSound()
{
    MIX_Audio* pew = MIX_LoadAudio(mixer, "Audio/laser.wav", true);
    MIX_PlayAudio(mixer, pew);
    MIX_DestroyAudio(pew);
}
