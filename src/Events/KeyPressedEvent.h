#ifndef KEYPRESSEDEVENT_H
#define KEYPRESSEDEVENT_H

#include "../EventBus/Event.h"
#include <SDL.h>

class KeyPressedEvent: public Event {
    public:
        SDL_KeyCode keyPressed; 

        KeyPressedEvent(SDL_KeyCode keyPressed): keyPressed(keyPressed) {}
        ~KeyPressedEvent() = default;
};

class KeyReleasedEvent: public Event {
    public:
        SDL_KeyCode keyPressed; 

        KeyReleasedEvent(SDL_KeyCode keyPressed): keyPressed(keyPressed) {}
        ~KeyReleasedEvent() = default;
};



#endif