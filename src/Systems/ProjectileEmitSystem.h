#ifndef PROJECTILEEMITSYSTEM
#define PROJECTILEEMITSYSTEM

#include "../ECS/ECS.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ProjectileComponent.h"


class ProjectileEmitSystem: public System {

    public:
        ProjectileEmitSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<ProjectileEmitterComponent>();
        }

        void SusbcribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::onKeyPressed);
        }

        void onKeyPressed(KeyPressedEvent& event) {
            if (event.keyPressed == SDLK_SPACE) {
                for (auto& entity: GetSystemEntities()) {
                    if (entity.HasTag("player")) {
                        const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                        const auto rigidBody = entity.GetComponent<RigidBodyComponent>();
                        const auto transform = entity.GetComponent<TransformComponent>();

                        //Add a new projectile, passing the registry
                        Entity projectile = entity.registry->CreateEntity();
                        projectile.Group("projectiles");

                        glm::vec2 projectilePosition = transform.position;

                        if (entity.HasComponent<SpriteComponent>()){
                            auto sprite = entity.GetComponent<SpriteComponent>();
                            projectilePosition.x += (sprite.width * transform.scale.x / 2);
                            projectilePosition.y += (sprite.height * transform.scale.y / 2);
                        }

                        glm::vec2 direction = glm::sign(rigidBody.velocity);
                        glm::vec2 projectileVelocity = projectileEmitter.velocity;
                        projectileVelocity.x = projectileEmitter.velocity.x * direction.x;
                        projectileVelocity.y = projectileEmitter.velocity.x * direction.y;
                        
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1, 1), 0);
                        projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                        projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0));
                        projectile.AddComponent<SpriteComponent>("bullet-texture", 2, false, 4, 4);
                        Logger::Error("Created a new one");

                        projectile.AddComponent<ProjectileComponent>(
                            projectileEmitter.isFriendly,
                            projectileEmitter.hitPercentDamage,
                            projectileEmitter.duration
                        );
                    }
                }
            }
        }

        //https://stackoverflow.com/questions/11277249/how-to-pass-stdunique-ptr-around
        void Update(Registry& registry, const double deltaTime) {
            for (auto entity: GetSystemEntities()) {
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                if (projectileEmitter.repeatFrequency == 0) {
                    continue;
                }
                const auto transform = entity.GetComponent<TransformComponent>();
                projectileEmitter.timeBetweenEmissions = projectileEmitter.timeBetweenEmissions + deltaTime * 1000.0f; //seconds
                
                // Logger::Warning("Entity " + std::to_string(entity.GetId()) + " emitted a projectile with time/fequency " + std::to_string(projectileEmitter.timeBetweenEmissions) + " " + std::to_string(projectileEmitter.repeatFrequency) );

                if (projectileEmitter.timeBetweenEmissions > projectileEmitter.repeatFrequency) {
                    
                    //Add a new projectile, passing the registry
                    Entity projectile = registry.CreateEntity();
                    projectile.Group("projectiles");

                    glm::vec2 projectilePosition = transform.position;
                    if (entity.HasComponent<SpriteComponent>()){
                        auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (sprite.width * transform.scale.x / 2);
                        projectilePosition.y += (sprite.height * transform.scale.y / 2);
                    }

                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1, 1), 0);
                    projectile.AddComponent<RigidBodyComponent>(projectileEmitter.velocity);
                    projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0));
                    projectile.AddComponent<SpriteComponent>("bullet-texture", 2, false, 4, 4);
                    projectile.AddComponent<ProjectileComponent>(
                        projectileEmitter.isFriendly,
                        projectileEmitter.hitPercentDamage,
                        projectileEmitter.duration
                    );

                    projectileEmitter.timeBetweenEmissions = 0.0f;
                }
            }
        }

};




#endif