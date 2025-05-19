#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL.h>
#include <sol/sol.hpp>

class LevelLoader {

    public:
        LevelLoader();
        ~LevelLoader();
        void LoadLevel(sol::state& lua, Registry& registry, AssetStore& assetStore, SDL_Renderer* renderer, int levelDif);
};

#endif