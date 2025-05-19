#ifndef RENDERTEXTSYSTEM_H
#define RENDERTEXTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TextLabelComponent.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>


class RenderTextSystem : public System {
    public:
        RenderTextSystem() {
            RequireComponent<TextLabelComponent>();
        }

        void Update(SDL_Renderer* renderer, SDL_Rect camera, const AssetStore& assetStore) {
            for (auto entity: GetSystemEntities()) {
                const auto textLabel = entity.GetComponent<TextLabelComponent>();
                SDL_Surface* surface = TTF_RenderText_Blended(
                    assetStore.GetFont(textLabel.assetId), 
                    textLabel.text.c_str(), 
                    textLabel.colour
                );

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                
                int labelWidth = 0;
                int labelHeight = 0;

                SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
                SDL_Rect dstRect = {
                    static_cast<int>(textLabel.position.x - camera.x * (int)(!textLabel.isFixed)),
                    static_cast<int>(textLabel.position.y - camera.y * (int)(!textLabel.isFixed)),
                    labelWidth, labelHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &dstRect);
                SDL_DestroyTexture(texture);
            }
        }       
};

#endif