#ifndef MOVEMENTSYSTEM
#define MOVEMENTSYSTEM

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

#include "../Events/CollisionEvent.h"

class MovementSystem : public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();

        }
        void SusbcribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
        }

        void onCollision(CollisionEvent& event){
            Entity a = event.lhs;
            Entity b = event.rhs;

            if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
                OnEnemyHitsObstacle(a, b); // "a" is the enemy, "b" is the obstacle
            }
            
            if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
                OnEnemyHitsObstacle(b, a); // "b" is the enemy, "a" is the obstacle
            }

            if (a.HasTag("player") && b.BelongsToGroup("obstacles")) {
                OnPlayerHitsObstacle(a, b); // "b" is the enemy, "a" is the obstacle
            }

            if (a.BelongsToGroup("obstacles") && b.HasTag("player")) {
                OnPlayerHitsObstacle(b, a); // "b" is the enemy, "a" is the obstacle
            }

        }

        void OnPlayerHitsObstacle(Entity entity, Entity obstacle) {
            if (
                entity.HasComponent<RigidBodyComponent>() \
                && entity.HasComponent<SpriteComponent>() \
                && entity.HasComponent<AnimationComponent>()
            ) {
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();

                if (rigidBody.velocity.x !=0) {
                    rigidBody.velocity.x *= -1;
                    sprite.srcRect.x = animation.currentFrame * sprite.width;
                }

                if (rigidBody.velocity.y !=0) {
                    rigidBody.velocity.y *= -1;
                    sprite.srcRect.x = animation.currentFrame * sprite.width;
                }
            }
        }

        void OnEnemyHitsObstacle(Entity entity, Entity obstacle) {
            if (entity.HasComponent<RigidBodyComponent>() && entity.HasComponent<SpriteComponent>()) {
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();

                if (rigidBody.velocity.x !=0) {
                    rigidBody.velocity.x *= -1;
                    sprite.flip = (sprite.flip==SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }

                if (rigidBody.velocity.y !=0) {
                    rigidBody.velocity.y *= -1;
                    sprite.flip = (sprite.flip==SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            }
        }

        // void Update(const double deltaTime);
        void Update(const double deltaTime){
            //Loop over all the system's entities 
            //that the system is intersted in

            int mapWidth = Game::mapInfo.numColumns * Game::mapInfo.tileSize * Game::mapInfo.tileScale;
            int mapHeight = Game::mapInfo.numRows * Game::mapInfo.tileSize * Game::mapInfo.tileScale;

            for (auto& entity: GetSystemEntities()){
                //Update the entity's position based on its velocity
                //for every frame of the game loop
                auto& transform = entity.GetComponent<TransformComponent>();
                const auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
        
                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;
                
                if (entity.HasTag("player")) {
                    float marginLeft = 10;
                    float margingRight = 50;
                    float marginTop = 10;
                    float marginBottom = 50;
    
                    transform.position = glm::clamp(
                        transform.position, 
                        glm::vec2(marginLeft, marginTop),
                        glm::vec2(mapWidth-margingRight, mapHeight-marginBottom)
                    );
                }
                
                const int margin = 100;
                //if the entity (like, a projectile) is outside the map,
                const bool isEntityOutsideMap = (
                    transform.position.x < -margin ||
                    transform.position.x > mapWidth + margin ||
                    transform.position.y < -margin ||
                    transform.position.y > mapHeight + margin
                );

                //one has to be killed
                if (isEntityOutsideMap && !entity.HasTag("player")) {
                    Logger::Warning("Killing the outside map entity " + std::to_string(entity.GetId()));
                    entity.Kill();
                }
            }
            
        }
};

#endif