#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/ProjectileEmitterComponent.h"

#include <tuple>

std::tuple<double, double> GetEntityPosition(Entity entity) {
    if (entity.HasComponent<TransformComponent>()) {
        const auto transform = entity.GetComponent<TransformComponent>();
        return std::make_tuple(transform.position.x, transform.position.y);
    } else {
        Logger::Error("Trying to get the position of an entity that has no transform component");
        return std::make_tuple(0.0, 0.0);
    }
}

std::tuple<double, double> GetEntityVelocity(Entity entity) {
    if (entity.HasComponent<RigidBodyComponent>()) {
        const auto rigidbody = entity.GetComponent<RigidBodyComponent>();
        return std::make_tuple(rigidbody.velocity.x, rigidbody.velocity.y);
    } else {
        Logger::Error("Trying to get the velocity of an entity that has no rigidbody component");
        return std::make_tuple(0.0, 0.0);
    }
}

void SetEntityPosition(Entity entity, double x, double y) {
    if (entity.HasComponent<TransformComponent>()) {
        auto& transform = entity.GetComponent<TransformComponent>();
        transform.position.x = x;
        transform.position.y = y;
    } else {
        Logger::Error("Trying to set the position of an entity that has no transform component");
    }
}

void SetEntityVelocity(Entity entity, double x, double y) {
    if (entity.HasComponent<RigidBodyComponent>()) {
        auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
        rigidbody.velocity.x = x;
        rigidbody.velocity.y = y;
    } else {
        Logger::Error("Trying to set the velocity of an entity that has no rigidbody component");
    }
}

void SetEntityRotation(Entity entity, double angle) {
    if (entity.HasComponent<TransformComponent>()) {
        auto& transform = entity.GetComponent<TransformComponent>();
        transform.rotation = angle;
    } else {
        Logger::Error("Trying to set the rotation of an entity that has no transform component");
    }
}

void SetEntityAnimationFrame(Entity entity, int frame) {
    if (entity.HasComponent<AnimationComponent>()) {
        auto& animation = entity.GetComponent<AnimationComponent>();
        animation.currentFrame = frame;
    } else {
        Logger::Error("Trying to set the animation frame of an entity that has no animation component");
    }
}

void SetProjectileVelocity(Entity entity, double x, double y) {
    if (entity.HasComponent<ProjectileEmitterComponent>()) {
        auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
        projectileEmitter.velocity.x = x;
        projectileEmitter.velocity.y = y;
    } else {
        Logger::Error("Trying to set the projectile velocity of an entity that has no projectile emitter component");
    }
}


class ScriptSystem: public System{

public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    void Update(double deltaTime, int elapsedTime) {
        //loop over all the entities and invoke their script functions
        for (auto entity: GetSystemEntities()) {
            const auto script = entity.GetComponent<ScriptComponent>();
            script.func(entity, deltaTime, elapsedTime); //invoking the lua function
        }
    }

    void CreateLuaBindings(sol::state& lua) {
        //create an "Entity" user type and expose it to Lua
        lua.new_usertype<Entity>(
            "entity", 
            "get_id", &Entity::GetId,
            "destroy", &Entity::Kill,
            "has_tag", &Entity::HasTag,
            "belongs_to_group", &Entity::BelongsToGroup
        );
        
        //exposing some of the glm vector API
        lua.new_usertype<glm::vec2>(
            "vec2",
            sol::constructors<glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );

        //create all the bindings between c++ and lua funtions
        lua.set_function("set_position", SetEntityPosition);
        lua.set_function("get_position", SetEntityPosition);

        lua.set_function("set_velocity", SetEntityPosition);
        lua.set_function("get_velocity", SetEntityPosition);

        lua.set_function("set_rotation", SetEntityPosition);

        lua.set_function("set_projectile_velocity", SetEntityPosition);

        lua.set_function("set_animation_frame", SetEntityPosition);
    }
};




#endif