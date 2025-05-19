#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include <SDL.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/CameraFollowComponent.h"

int clamp(int value, int l, int h){
    return std::max(l, std::min(value, h));
}

class CameraMovementSystem : public System {
    public:
        CameraMovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<CameraFollowComponent>();
        }

        void Update(SDL_Rect& camera) {
            for (auto entity: GetSystemEntities()) {
                auto transform = entity.GetComponent<TransformComponent>();
                
                int mapWidth = Game::mapInfo.numColumns * Game::mapInfo.tileSize * Game::mapInfo.tileScale;
                int mapHeight = Game::mapInfo.numRows * Game::mapInfo.tileSize * Game::mapInfo.tileScale;
   
                // if (transform.position.x < mapWidth) {   
                //     camera.x = transform.position.x - (Game::windowWidth / 2);
                // }

                // if (transform.position.y < mapHeight) {
                //     camera.y = transform.position.y - (Game::windowHeight / 2);
                
                // }

                // camera.x = camera.x < 0 ? 0 : camera.x;
                // camera.y = camera.x < 0 ? 0 : camera.y;
                // camera.x = camera.x > camera.w ? camera.w : camera.x;
                // camera.y = camera.y > camera.h ? camera.h : camera.y;

                camera.x = clamp(
                    transform.position.x - (camera.w / 2),
                    0,
                    mapWidth - camera.w
                );

                camera.y = clamp(
                    transform.position.y - (camera.h / 2),
                    0,
                    mapHeight - camera.h
                );

            }
        }
};

#endif