#ifndef ANIMATIONSYSTEM
#define ANIMATIONSYSTEM

#include "../ECS/ECS.h"
#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"

class AnimationSystem: public System{
    public:
        AnimationSystem(){
            RequireComponent<AnimationComponent>();
            RequireComponent<SpriteComponent>();
        }
        
        void Update(const double deltaTime) {
            for (auto entity: GetSystemEntities()){
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();

                animation.elapsedTime += deltaTime;

                if (animation.isLoop){
                    animation.currentFrame = (
                       (int) (animation.elapsedTime * animation.frameSpeedRate)
                    ) % animation.numFrames;
            
                } else {
                    animation.currentFrame = std::min(
                        (int) (animation.elapsedTime * animation.frameSpeedRate),
                        animation.numFrames - 1
                    );
                }
                
                //horizontal sprite progression
                sprite.srcRect.x = animation.currentFrame * sprite.width;

            }
        }
};  

#endif