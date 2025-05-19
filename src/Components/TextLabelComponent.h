#ifndef TEXTLABELCOMPONENT_H
#define TEXTLABELCOMPONENT_H

#include <glm/glm.hpp>
#include <string>
#include <SDL.h>

struct TextLabelComponent {
    /* data */
    glm::vec2 position;
    std::string text;
    std::string assetId;
    SDL_Color colour;
    bool isFixed;

    TextLabelComponent(
        glm::vec2 position=glm::vec2(0),
        std::string text="",
        std::string assetId="",
        const SDL_Color& colour = {0, 0, 0, 255},
        bool isFixed=true
    ) {
        this->position=position;
        this->text=text;
        this->assetId=assetId;
        this->colour=colour;
        this->isFixed=isFixed;

    }
};


#endif