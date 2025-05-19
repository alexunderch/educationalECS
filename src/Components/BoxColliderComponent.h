#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include <glm/glm.hpp>
#include <SDL.h>

struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset;
    SDL_Color colour;
    //hardcoding them into the constructor

    BoxColliderComponent(int width=0, int height=0, glm::vec2 offset=glm::vec2(0, 0)){
        this->width=width;
        this->height=height;
        this->offset=offset;
        // this->colour=CASUAL;
    }
};


#endif