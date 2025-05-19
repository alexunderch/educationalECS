#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"


#include "../Systems/CollisionSystem.h"
#include "../EventBus/EventBus.h"

class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }

        void SusbcribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
        }

        void onCollision(CollisionEvent& event){
            Entity a = event.lhs;
            Entity b = event.rhs;
           
            if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
                Logger::Error("Projectile hit the player");
                OnProjectileHitsPlayer(a, b);
            }

            if (a.HasTag("player") && b.BelongsToGroup("projectiles")) {
                Logger::Error("Player hit projectile");
                OnProjectileHitsPlayer(b, a);
            }

            if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
                OnProjectileHitsEnemy(a, b);
            }

            if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
                OnProjectileHitsEnemy(b, a);
                
            }
        }

        void OnProjectileHitsPlayer(Entity projectile, Entity player) {
            auto& projectileComponent = projectile.GetComponent<ProjectileComponent>();
            if (!projectileComponent.isFriendly){
                auto& health = player.GetComponent<HealthComponent>();
                //do damage

                health.healthPercentage -= projectileComponent.hitPercentDamage;
                Logger::Warning("Player got damaged by " + std::to_string(health.healthPercentage));

                //kill if the damage is lethal 
                if (health.healthPercentage<=0) {
                    player.Kill();
                }
                projectile.Kill();
            }
            
        }

        void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {

            auto& projectileComponent = projectile.GetComponent<ProjectileComponent>();
            if (projectileComponent.isFriendly){
                auto& health = enemy.GetComponent<HealthComponent>();
                //do damage
                Logger::Warning("Player got damaged by " + std::to_string(health.healthPercentage));

                health.healthPercentage -= projectileComponent.hitPercentDamage;
                //kill if the damage is lethal 
                if (health.healthPercentage<=0) {
                    enemy.Kill();
                }
                projectile.Kill();
            }
        }

        void Update() {

        }
};


#endif