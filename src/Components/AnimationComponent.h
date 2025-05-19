#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <SDL.h>

struct AnimationComponent{
    int numFrames;
    int currentFrame;
    int frameSpeedRate;
    bool isLoop;
    double elapsedTime;


    AnimationComponent(
        int numFrames=0, 
        int frameSpeedRate=0,
        bool isLoop=false
    ){
        this->numFrames=numFrames;
        this->currentFrame=1;
        this->frameSpeedRate=frameSpeedRate;
        this->isLoop=isLoop;
        this->elapsedTime=0;
    }
};

#endif