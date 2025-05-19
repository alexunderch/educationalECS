#ifndef KEYBOARDMOVEMENTSYSTEM_H
#define KEYBOARDMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

#include "../Components/KeyboardControlledComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"


class KeyboardMovementSystem: public System {
    public:
        KeyboardMovementSystem() {
            //Requires absolutely nothing
            RequireComponent<KeyboardControlledComponent>();
            RequireComponent<RigidBodyComponent>();
            RequireComponent<SpriteComponent>();
        }

        void SusbcribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onKeyPressed);
            // eventBus->SubscribeToEvent<KeyReleasedEvent>(this, &KeyboardMovementSystem::onKeyReleased);

        }

        void onKeyPressed(KeyPressedEvent& event){
            // auto keyPressed = std::string(SDL_GetKeyName(event.keyPressed));
            for (auto entity: GetSystemEntities()) {
                const auto keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
                
                switch (event.keyPressed)
                {
                case (SDLK_UP):
                    rigidBody.velocity = keyboardControl.upVelocity;
                    sprite.srcRect.y = sprite.height * 0;
                    break;

                case (SDLK_RIGHT):
                    rigidBody.velocity = keyboardControl.rightVelocity;
                    sprite.srcRect.y = sprite.height * 1;
                    break;

                case (SDLK_DOWN):
                    rigidBody.velocity = keyboardControl.downVelocity;
                    sprite.srcRect.y = sprite.height * 2;
                    break;


                case (SDLK_LEFT):
                    rigidBody.velocity = keyboardControl.leftVelocity;
                    sprite.srcRect.y = sprite.height * 3;
                    break;
                
                default:
                    break;
                }

            }
            
        }

        void onKeyReleased(KeyReleasedEvent& event){
            // https://stackoverflow.com/questions/26235149/sdl-movement-stops
            //TODO: handle release smoothly
        }


        void Update() {}
};

#endif