#include <Entities/Guns.h>
#include <Utils/MemoryMonitor.h>
Guns::Guns(){
	currentGunType = Guns::GunType::PISTOL;
	mixer = nullptr;
}

Guns::~Guns()
{
 
}

void Guns::setGunType(GunType type){
	switch(type){
		case Guns::GunType::PISTOL:
			std::cout << "Gun type set to PISTOL" << std::endl;
			currentGunType = Guns::GunType::PISTOL;
			break;
		case Guns::GunType::SHOTGUN:
			std::cout << "Gun type set to SHOTGUN" << std::endl;
			currentGunType = Guns::GunType::SHOTGUN;
			break;
		case Guns::GunType::RIFLE:
			std::cout << "Gun type set to RIFLE" << std::endl;
			currentGunType = Guns::GunType::RIFLE;
			break;
		default:
			std::cout << "Unknown gun type" << std::endl;
			break;
	}
}

std::vector<std::unique_ptr<Actor2D>> Guns::shoot(PlayerController* controller, std::vector<std::unique_ptr<Actor2D>>& impacts)
{
    std::vector<std::unique_ptr<Actor2D>> bullets;

    const Vec3 pos = controller->GetCrossHairsPosition();
    const auto quat = controller->GetCrossHairsRotation();
    const Vec3 forward(1.0f, 0.0f, 0.0f);

    switch (currentGunType) {
    case Guns::GunType::PISTOL: {
        const Vec3 velocity = quat * forward * QMath::inverse(quat) * 100.0f;

        auto bullet = std::make_unique<Actor2D>();
        bullet->OnCreate("sprites/fatty_clicked.png");
        bullet->GetEntity()->SetPosition(pos);
        bullet->GetEntity()->AdjustOrientation(quat);
        bullet->GetEntity()->SetVelocity(velocity);
        bullet->SetTag(Actor_tags::bullets);
        bullet->ConfigureLifeSpan(2.0f);

        bullet->RegisterExpiredCallback([&impacts](const Actor2D& actor) {
            auto impact = std::make_unique<Actor2D>();
            impact->OnCreate("sprites/impact.png");
            impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
            impact->ConfigureLifeSpan(1.0f);
            impacts.push_back(std::move(impact));
        });
        if (funnyNoises) { playShootSound(); }
        bullets.push_back(std::move(bullet));
        break;
    }

    case Guns::GunType::SHOTGUN: {
        for (int i = 0; i < 5; ++i) {
            Vec3 spreadPos = pos + Vec3(static_cast<float>(rand() % 21 - 10), static_cast<float>(rand() % 21 - 10), 0.0f);
            const Vec3 velocity = quat * forward * QMath::inverse(quat) * 100.0f;

            auto bullet = std::make_unique<Actor2D>();
            bullet->OnCreate("sprites/fatty_clicked.png");
            bullet->GetEntity()->SetPosition(spreadPos);
            bullet->GetEntity()->AdjustOrientation(quat);
            bullet->GetEntity()->SetVelocity(velocity);
            bullet->SetTag(Actor_tags::bullets);
            bullet->ConfigureLifeSpan(2.0f);

            bullet->RegisterExpiredCallback([&impacts](const Actor2D& actor) {
                auto impact = std::make_unique<Actor2D>();
                impact->OnCreate("sprites/impact.png");
                impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
                impact->ConfigureLifeSpan(1.0f);
                impacts.push_back(std::move(impact));
            });
			if (funnyNoises) { playShootSound(); }
            bullets.push_back(std::move(bullet));
            
        }
        break;
    }

    case Guns::GunType::RIFLE: {
        // Rifle is a faster single bullet with longer lifespan and higher speed
        const Vec3 velocity = quat * forward * QMath::inverse(quat) * 200.0f; // faster bullet

        auto bullet = std::make_unique<Actor2D>();
        bullet->OnCreate("sprites/fatty_clicked.png");
        bullet->GetEntity()->SetPosition(pos);
        bullet->GetEntity()->AdjustOrientation(quat);
        bullet->GetEntity()->SetVelocity(velocity);
        bullet->SetTag(Actor_tags::bullets);
        bullet->ConfigureLifeSpan(3.0f); // longer lifespan

        bullet->RegisterExpiredCallback([&impacts](const Actor2D& actor) {
            auto impact = std::make_unique<Actor2D>();
            impact->OnCreate("sprites/impact.png");
            impact->GetEntity()->SetPosition(actor.GetEntity()->GetPosition());
            impact->ConfigureLifeSpan(1.0f);
            impacts.push_back(std::move(impact));
            });
        if (funnyNoises) { playShootSound(); }
        bullets.push_back(std::move(bullet));
        break;
    }

    default:
        break;
    }

    return bullets;
}

void Guns::playShootSound()
{
    MIX_Audio* pew = MIX_LoadAudio(mixer, "Audio/laser.wav", true);
    MIX_PlayAudio(mixer, pew);
    MIX_DestroyAudio(pew);
}
