#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

#include <SDL.h>

bool ABABCollisionCheck(
    const TransformComponent& firstTransform,
    const TransformComponent& secondTransform, 
    const BoxColliderComponent& firstBoxCollider,
    const BoxColliderComponent& secondBoxCollider
){
    // https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection

    return (
        firstTransform.position.x + firstBoxCollider.offset.x * firstTransform.scale.x < secondTransform.position.x + (secondBoxCollider.offset.x  + firstBoxCollider.width) * secondTransform.scale.x &&
        firstTransform.position.x + (firstBoxCollider.offset.x + firstBoxCollider.width) * firstTransform.scale.x > secondBoxCollider.offset.x *  secondTransform.scale.x + secondTransform.position.x &&
        firstBoxCollider.offset.y * firstTransform.scale.y + firstTransform.position.y < secondTransform.position.y + (secondBoxCollider.offset.y + secondBoxCollider.height) * secondTransform.scale.y &&
        (firstBoxCollider.offset.y + firstTransform.position.y + firstBoxCollider.height) * firstTransform.scale.y > secondTransform.position.y + secondBoxCollider.offset.y * secondTransform.scale.y
    );
}


bool ABABCollisionCheckSDL(
    SDL_Renderer* renderer,
    const TransformComponent& firstTransform,
    const TransformComponent& secondTransform, 
    const BoxColliderComponent& firstBoxCollider,
    const BoxColliderComponent& secondBoxCollider
){
    // https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection

    SDL_Rect firstBoundingBox = {
        static_cast<int>(firstTransform.position.x + firstBoxCollider.offset.x * firstTransform.scale.x),
        static_cast<int>(firstTransform.position.y + firstBoxCollider.offset.y * firstTransform.scale.y),
        static_cast<int>(firstBoxCollider.width * firstTransform.scale.x),
        static_cast<int>(firstBoxCollider.height * firstTransform.scale.y),
    };

    SDL_Rect secondBoundingBox = {
        static_cast<int>(secondTransform.position.x + secondBoxCollider.offset.x * secondTransform.scale.x),
        static_cast<int>(secondTransform.position.y + secondBoxCollider.offset.y * secondTransform.scale.y),
        static_cast<int>(secondBoxCollider.width * secondTransform.scale.x),
        static_cast<int>(secondBoxCollider.height * secondTransform.scale.y),
    };

    bool collisionHappened = static_cast<bool>(SDL_HasIntersection(&firstBoundingBox, &secondBoundingBox));
    return collisionHappened;
}

class CollisionSystem: public System {
    public:
        SDL_Color CASUAL =  {81, 255, 0, 255};
        SDL_Color COLLISION = {255, 0, 0, 255};

        CollisionSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();

        }

        void Draw(SDL_Renderer* renderer, SDL_Rect& camera) {

            for (auto entity: GetSystemEntities()){
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& boxCollider = entity.GetComponent<BoxColliderComponent>();


                SDL_Rect boundingBox = {
                    static_cast<int>(transform.position.x + boxCollider.offset.x * transform.scale.x - camera.x),
                    static_cast<int>(transform.position.y + boxCollider.offset.y * transform.scale.y - camera.y),
                    static_cast<int>(boxCollider.width * transform.scale.x),
                    static_cast<int>(boxCollider.height * transform.scale.y),
                };
                auto& bboxColor = boxCollider.colour;

                SDL_SetRenderDrawColor(renderer, bboxColor.r, bboxColor.g, bboxColor.b, bboxColor.a);
                SDL_RenderDrawRect(renderer, &boundingBox);
            }

        }

        void Update(std::unique_ptr<EventBus>& eventBus) {
            
            auto entities = GetSystemEntities();

            for (auto first = entities.begin(); first != entities.end(); ++first){
                auto& firstTransform = first->GetComponent<TransformComponent>();
                auto& firstBoxCollider = first->GetComponent<BoxColliderComponent>();
                
                for (auto second = first + 1; second != entities.end(); ++second){
                    auto& secondTransform = second->GetComponent<TransformComponent>();
                    auto& secondBoxCollider = second->GetComponent<BoxColliderComponent>();   
                    
                    bool collisionHappened = ABABCollisionCheck(firstTransform, secondTransform, firstBoxCollider, secondBoxCollider);
                    
                    if (collisionHappened){ 
                        firstBoxCollider.colour=COLLISION;
                        secondBoxCollider.colour=COLLISION;
                        eventBus->EmitEvent<CollisionEvent>(*first, *second);
                    } else {
                        firstBoxCollider.colour=CASUAL;
                        secondBoxCollider.colour=CASUAL;
                    }
                }
            }
        }
};


#endif 