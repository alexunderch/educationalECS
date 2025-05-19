#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL.h>
#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../AssetStore/AssetStore.h"

class RenderSystem : public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(SDL_Renderer* renderer, const AssetStore& assetStore, SDL_Rect& camera){
            //TODO: sort the entites by the z-index: every frame? — depending on the nature of the game
            struct RenderableEntity{
                SpriteComponent spriteComponent;
                TransformComponent transformComponent;
            };

            std::vector<RenderableEntity> renderableEntities;
            for (auto& entity: GetSystemEntities()) {
                RenderableEntity renderableEntity;
                renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
                renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();

                //bypass the entities if they are outside the camera view (and not fixed)
                //thus, not rendering them in this case 
                bool isEntityOutsideCameraView = (
                    renderableEntity.transformComponent.position.x + (renderableEntity.spriteComponent.width * renderableEntity.transformComponent.scale.x)  < camera.x ||
                    renderableEntity.transformComponent.position.x > camera.x + camera.w ||
                    renderableEntity.transformComponent.position.y + (renderableEntity.spriteComponent.height * renderableEntity.transformComponent.scale.y) < camera.y ||
                    renderableEntity.transformComponent.position.y > camera.y + camera.h
                );
                if (isEntityOutsideCameraView && !renderableEntity.spriteComponent.isFixed) {
                    continue;
                }
                renderableEntities.emplace_back(renderableEntity);
            }

            std::sort(
                renderableEntities.begin(), 
                renderableEntities.end(),
                [](const RenderableEntity& lhs, const RenderableEntity& rhs) {
                    return lhs.spriteComponent.zIndex < rhs.spriteComponent.zIndex; 
                }
            );

            //Renders every frame
            for (auto entity: renderableEntities){
                const auto& transform = entity.transformComponent;
                auto& sprite = entity.spriteComponent;
                
                SDL_Rect srcRect = sprite.srcRect;
                SDL_Texture* texture = assetStore.GetTexture(sprite.assetId);

                if (srcRect.h == 0 && srcRect.w == 0) {
                    //automatically infer the texture size if
                    //one isn't specified
                    SDL_QueryTexture(
                        texture,
                        NULL,
                        NULL,
                        &srcRect.w,
                        &srcRect.h
                    );
                }

                if (sprite.width == 0 && sprite.height == 0) {
                    sprite.width = srcRect.w;
                    sprite.height = srcRect.h;
                }
               
                SDL_Rect destRect = {
                    static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
                    static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y),
                };
               
                SDL_RenderCopyEx(
                    renderer,
                    texture,
                    &srcRect,
                    &destRect,
                    transform.rotation,
                    NULL, //rotation around the centre point
                    sprite.flip
                );

            }
        }
};


#endif