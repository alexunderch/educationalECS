#ifndef EVENTBUS_H
#define EVENTBUS_H

#include "Event.h"
#include <map>
#include <memory>
#include <list>
#include <typeindex>

class IEventCallback {
    private:
        virtual void Call(Event& e) = 0;

    public:
        virtual ~IEventCallback() = default;

        void Execute(Event& e) {
            Call(e);
        }
};  

template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
    //callback pointer to the funtion
    //that has to be invoked

    private:
        typedef void (TOwner::*CallBackFunction)(TEvent&);
        TOwner* ownerInstance;
        CallBackFunction callBackFunction;

        virtual void Call(Event& e) override {
            std::invoke(callBackFunction, ownerInstance, static_cast<TEvent&>(e));
        }
    public:
        EventCallback(TOwner* ownerInstance, CallBackFunction callBackFunction) {
            this->ownerInstance=ownerInstance;
            this->callBackFunction=callBackFunction;
        }
        virtual ~EventCallback() override = default;
};

using HandlerList = std::list<std::unique_ptr<IEventCallback>>;

class EventBus {
    //TypeIndex -> list of function pointers [callbacks] 
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
    
    public:
        EventBus() {
            Logger::Log("Event bus contructor was called");
        }
        ~EventBus() {
            Logger::Log("Event bus destructor was called");
        }

        //clear the subscriber's list 
        void Reset() {
            subscribers.clear();
        }

        //or ListenToEvent (event type <T>)
        template <typename TEvent, typename TOwner>
        void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)){
            if (subscribers[typeid(TEvent)].get() == nullptr) {
                //nothing there to subscribe
                subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
            }

            auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);            
            //why std::move...since it's a unique_ptr and we need to transfer the ownership
            subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
        }
        // or DispatchEvent
        template <typename TEvent, typename ...TArgs>
        void EmitEvent(TArgs&& ...args){
            auto handlers = subscribers[typeid(TEvent)].get();
            if (handlers) {
                //loop over all subscribers
                for (auto it = handlers->begin(); it!=handlers->end(); it++) {
                    auto handler = it->get();
                    //Trigger the callback
                    TEvent event(std::forward<TArgs>(args)...);
                    handler->Execute(event);
                }
            }
        }
        
};

#endif
