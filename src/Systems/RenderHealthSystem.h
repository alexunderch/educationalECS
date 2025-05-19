#ifndef RENDERHEALTHSYSTEM_H
#define RENDERHEALTHSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>

typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;


RgbColor HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;
    
    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }
    
    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 
    
    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;
    
    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }
    
    return rgb;
}

HsvColor RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);
    
    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}

unsigned char linearInterp(double t, unsigned char a, unsigned char b) {
    // linear interp (endValue - startValue) * stepNumber / lastStepNumber + startValue;
    double aNorm = a;
    double bNorm = b;
    return static_cast<unsigned char>(aNorm * (1. - t) + bNorm * t);
}

SDL_Color getColour(double proportion) {
    SDL_Color healthyColour{0, 255, 0, 255};
    SDL_Color injuredColour{255, 0, 0, 255};

    HsvColor hsvA = RgbToHsv({healthyColour.r, healthyColour.g, healthyColour.b});
    HsvColor hsvB = RgbToHsv({injuredColour.r, injuredColour.g, injuredColour.b});

    RgbColor interp  = HsvToRgb({
        linearInterp(1. - proportion, hsvA.h, hsvB.h),
        linearInterp(1. - proportion, hsvA.s, hsvB.s),
        linearInterp(1. - proportion, hsvA.v, hsvB.v)
    });

    return {
        interp.r, interp.g, interp.b, 255
    };

}


class RenderHealthSystem: public System {
    public:
        RenderHealthSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<HealthComponent>();
            RequireComponent<SpriteComponent>(); //to draw around the sprite, ig; it's not necessary
        }
    
        void Update(SDL_Renderer* renderer, SDL_Rect camera, const AssetStore& assetStore) {
            for (auto entity: GetSystemEntities()) {
                
                const auto sprite = entity.GetComponent<SpriteComponent>();
                const auto health = entity.GetComponent<HealthComponent>();
                //convert, then do mathematical operations
                double proportion = (double)health.healthPercentage / 100;
            
                const auto transform = entity.GetComponent<TransformComponent>();
                //rendering the bar
                int healthBarWidth = sprite.width - 2;
                int healthBarHeight = 7;
                double healthBarPosX = transform.position.x - camera.x;
                double healthBarPosY = (transform.position.y - (sprite.height / 4 * transform.scale.y)) - camera.y;
                
                SDL_Rect healthBarRect = {
                    static_cast<int>(healthBarPosX),
                    static_cast<int>(healthBarPosY),
                    static_cast<int>(healthBarWidth * proportion),
                    static_cast<int>(healthBarHeight)
                };

                SDL_Color healhBarColour = getColour(proportion);

                SDL_SetRenderDrawColor(
                    renderer, 
                    healhBarColour.r, 
                    healhBarColour.g, 
                    healhBarColour.b, 
                    healhBarColour.a
                );
                SDL_RenderFillRect(renderer, &healthBarRect);


                SDL_Surface* surface = TTF_RenderText_Blended(
                    assetStore.GetFont("healthbar-font"), 
                    std::to_string(health.healthPercentage).c_str(), 
                    healhBarColour
                );

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                
                int labelWidth = 0;
                int labelHeight = 0;

                SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
                SDL_Rect dstRect = {
                    static_cast<int>(healthBarPosX + (sprite.width * transform.scale.x) + 15),
                    static_cast<int>(healthBarPosY),
                    labelWidth, labelHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &dstRect);
                SDL_DestroyTexture(texture);
            }
        }       


};

#endif

