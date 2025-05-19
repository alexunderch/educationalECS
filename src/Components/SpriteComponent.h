#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include<string>
#include<SDL.h>

struct SpriteComponent {
    std::string assetId;
    int width;
    int height;
    bool isFixed;
    int zIndex; //css inspired idea of the layering = painter's algorithm
    SDL_Rect srcRect;
    SDL_RendererFlip flip;

    SpriteComponent(std::string assetId="", int zIndex=0, bool isFixed=false, int width=0, int height=0, int srcRectX=0, int srcRectY=0) {
        this->assetId=assetId;
        this->width=width;
        this->height=height;
        this->zIndex=zIndex;
        this->isFixed=isFixed;
        this->srcRect={srcRectX, srcRectY, width, height};
        this->flip=SDL_FLIP_NONE;
    }
};


#endif