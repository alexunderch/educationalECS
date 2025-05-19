#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H

#include <glm/glm.hpp>
#include <SDL.h>

struct ProjectileEmitterComponent {
    glm::vec2 velocity;
    int repeatFrequency;
    int duration;
    int hitPercentDamage;
    bool isFriendly;
    double timeBetweenEmissions;

    ProjectileEmitterComponent(
        glm::vec2 velocity=glm::vec2(0),
        int repeatFrequency=0,
        int duration=10000, //10 seconds
        int hitPercentDamage=10,
        bool isFriendly=false
    ) {
        this->velocity=velocity;
        this->repeatFrequency=repeatFrequency;
        this->duration=duration;
        this->hitPercentDamage=hitPercentDamage;
        this->isFriendly=isFriendly;
        this->timeBetweenEmissions=0;
    }
};


#endif