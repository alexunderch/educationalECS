#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"

class ProjectileLifecycleSystem: public System {

public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update(const double deltaTime) {
        for (auto entity: GetSystemEntities()) {
            auto& projectile = entity.GetComponent<ProjectileComponent>();
            projectile.elapsedTime += (deltaTime * 1000.0f);
           
            //the projectile reached its duration limit
            if (projectile.elapsedTime >= projectile.duration) {
                Logger::Error("Killing entity with id = " + std::to_string(entity.GetId()));
                entity.Kill();
                projectile.elapsedTime = 0.0f;
            }
        }
    }

};


#endif