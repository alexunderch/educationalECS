#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include <SDL.h>

class CollisionEvent: public Event {

    public:
        Entity lhs;
        Entity rhs;
        
        CollisionEvent(Entity lhs, Entity rhs): lhs(lhs), rhs(rhs) {};
        ~CollisionEvent() = default;
    };


#endif
