#include "Game.h"
#include "LevelLoader.h"

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthSystem.h"
#include "../Systems/RenderGUISystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/KeyboardMovementSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/ScriptSystem.h"

#include "../Events/KeyPressedEvent.h"

#include <SDL.h>
#include <SDL_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <iostream>

int Game::windowWidth;
int Game::windowHeight;
MapInfo Game::mapInfo;

Game::Game(){
    isRunning = false;
    visualizeColliders = false;
    renderGUI = false;
    
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
}

Game::~Game(){}

void Game::Initialise(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cerr << "Error when initialising SDL2" << std::endl;
        return;
    }

    if (TTF_Init() != 0) {
        Logger::Error("Error when initialising TTF!");
        return;
    }

    SDL_DisplayMode displayMode;
    //current display, fake fullscreen
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
    //title, x, y, (centre) w, h, flags
    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        windowWidth, 
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );

    if (!window){
        std::cerr << "Error when creating SDL window" << std::endl;
        return;
    }

    //window*, diplay_ind
    renderer = SDL_CreateRenderer(
        window,
        -1, //idc
        //dedicated gpu rendering | vsync
        0 //SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer){
        std::cerr << "Error when creating SDL renderer" << std::endl;
        return;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    //Initialising ImGUI and its renderer
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //Creating ImGUI IO and initialising the keyboard
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    //Initialising our camera
    camera.x = 0;
    camera.y = 0;
    camera.w = windowWidth;
    camera.h = windowHeight;

    isRunning = true;
}

void Game::Destroy(){
    //ImGUI Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    //order is reverse to creating
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    assetStore->ClearAssets();
    TTF_Quit();
    SDL_Quit();
}
void Game::Setup(){
    //Creating Systems that would process our entities equipped with Components
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<RenderHealthSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardMovementSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<ProjectileLifecycleSystem>();
    registry->AddSystem<ScriptSystem>();

    //imgui rendering for debugging
    registry->AddSystem<RenderGUISystem>();

    //Setup lua bindings
    registry->GetSystem<ScriptSystem>().CreateLuaBindings(lua);
    
    // Initialisation of the game objects, loadind the first level
    LevelLoader loader;
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);

    loader.LoadLevel(lua, *registry, *assetStore, renderer, 2);
  }

void Game::Run(){
    Setup();
    // Update loop;
    while(isRunning){
        ProcessInput();
        Update();
        Render();
    }
}
void Game::ProcessInput(){

    SDL_Event sdlEvent;

    SDL_StopTextInput(); // Stop text input first.
    //address of the event!
    while (SDL_PollEvent(&sdlEvent)){

        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();
        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        if (sdlEvent.type==SDL_KEYDOWN){
            eventBus->EmitEvent<KeyPressedEvent>(
                static_cast<SDL_KeyCode>(sdlEvent.key.keysym.sym)
            );
        }

        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            
            case SDL_KEYDOWN:
                
                switch (sdlEvent.key.keysym.sym) {
            
                    case SDLK_ESCAPE:
                        isRunning = false;
                        break;

                    case SDLK_c:
                        visualizeColliders = !visualizeColliders;
                        break;

                    case SDLK_g:
                        renderGUI = !renderGUI;
                        break;

                    default:
                        break;
                }
            case SDL_KEYUP: 
                eventBus->EmitEvent<KeyReleasedEvent>(
                    static_cast<SDL_KeyCode>(sdlEvent.key.keysym.sym)
                );
                break;

            default:
                break;
            }
    }

}
void Game::Update(){
    //if we are too fast, waste some time until we reach the MS_PER_FRAME
    // 1) inefficient way, overloading the cpu
    // while (!SDL_TICKS_PASSED(
    //     SDL_GetTicks(), 
    //     millisecondsPreviousFrame + MILLISECONDS_PER_FRAME
    // )); //wasting time, capping the framerate

    int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME){
        SDL_Delay(timeToWait);
    } 
    //differce in ticks since the last frame, converted to pixels per second
    double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0f;
    millisecondsPreviousFrame = SDL_GetTicks(); //ticks=milliseconds 

    //reseting all event handlers for the current frame:
    eventBus->Reset();

    //subscription of the events to the systems
    //questionable choice is subscribe during the update loop...
    registry->GetSystem<MovementSystem>().SusbcribeToEvents(eventBus); //collision w/ obstacles
    registry->GetSystem<DamageSystem>().SusbcribeToEvents(eventBus);
    registry->GetSystem<KeyboardMovementSystem>().SusbcribeToEvents(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SusbcribeToEvents(eventBus);


    //Asking the systems to update
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update(deltaTime);
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<CameraMovementSystem>().Update(camera);
    registry->GetSystem<ProjectileEmitSystem>().Update(*registry, deltaTime);
    registry->GetSystem<ProjectileLifecycleSystem>().Update(deltaTime);
    registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());

    //At the end of the frame, update the registry 
    registry->Update();

}

void Game::Render(){
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, *assetStore, camera);
    if (visualizeColliders) {
        registry->GetSystem<CollisionSystem>().Draw(renderer, camera);
    }

    if (renderGUI) {
        registry->GetSystem<RenderGUISystem>().Update(renderer, *registry, camera, *assetStore);
    }

    registry->GetSystem<RenderTextSystem>().Update(renderer, camera, *assetStore);
    registry->GetSystem<RenderHealthSystem>().Update(renderer, camera, *assetStore);

    SDL_RenderPresent(renderer);

}