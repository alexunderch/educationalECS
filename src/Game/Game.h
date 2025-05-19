// #pragma once
// protection guard
#ifndef GAME_H
#define GAME_H

#include <SDL.h>

#include <glm/glm.hpp>
#include <sol/sol.hpp>

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

struct MapInfo {
    size_t numRows;
    size_t numColumns;
    size_t tileSize;
    size_t tileScale;


    MapInfo(size_t numRows=1, size_t numColumns=1, size_t tileSize=1, size_t tileScale=1) {
        this->numRows=numRows;
        this->numColumns=numColumns;
        this->tileSize=tileSize;
        this->tileScale=tileScale;

    }
};


class Game {
    private:
        sol::state lua;

        bool isRunning;
        bool visualizeColliders;
        bool renderGUI;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Rect camera;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> assetStore;
        std::unique_ptr<EventBus> eventBus;

        int millisecondsPreviousFrame=0;

    public:
        Game();
        ~Game();
        void Initialise();
        void Setup();
        void Run();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();

        static int windowWidth;
        static int windowHeight;
        static MapInfo mapInfo;
};

#endif