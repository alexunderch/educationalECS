#ifndef SCRIPTCOMPONENT_H
#define SCRIPTCOMPONENT_H

#include <sol/sol.hpp>

struct ScriptComponent {
    public:
        sol::function func; //lua function to be called
        //this component violates the ECS paradigm because it has
        //logic inside!

        ScriptComponent(sol::function func = sol::lua_nil) {
            this->func=func;
        }
};

#endif