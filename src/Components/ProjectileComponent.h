#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

struct ProjectileComponent {
    bool isFriendly;
    int hitPercentDamage;
    int duration;
    double elapsedTime;

    ProjectileComponent(
        bool isFriendly=false,
        int hitPercentDamage=0,
        int duration=0
    ) {
        this->isFriendly=isFriendly;
        this->hitPercentDamage=hitPercentDamage;
        this->duration=duration;
        this->elapsedTime=0;
    }
};


#endif