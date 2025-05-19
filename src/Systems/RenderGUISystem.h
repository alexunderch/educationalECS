#ifndef RENDERGUISYSTEM
#define RENDERGUISYSTEM

// Sources:
// 1. The best demo I have ever seen: https://files.cdn.thinkific.com/file_uploads/167815/attachments/c90/a27/b8d/RenderGUISystem.h
// 2. The main demo https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp#L2451

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/SpriteComponent.h"



#include "SDL.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

class RenderGUISystem: public System{
public:
    RenderGUISystem() = default;

    void Update(SDL_Renderer* renderer, Registry& registry, const SDL_Rect& camera, AssetStore& assetStore) {
        //drawing the imgui objects

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        //creating a sample dialogue window with ImGui
        //flags
        // ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav; //e.g.
        if (ImGui::Begin("Spawning enemies")) {
            //ImGui objects are here

            //PLACEHODERS
            //transform parameters
            static int positionX=0, positionY=0;
            static int scaleX=1, scaleY=1;
            static float rotationDeg=0;

            //rigidbody parameters
            static int velocityX=0, velocityY=0;

            //placeholder for all the sprites
            std::vector<std::string> textures =  assetStore.ListTextures();
            const char* sprites[textures.size()]; 

            for (size_t i=0; i<textures.size(); i++) {
                sprites[i]=textures[i].c_str();
            }

            static int selectedSpriteIdx = 0;

            //projectile parameters
            static float projectileSpeed=0, projectileAngle=0;
            static int projectileRepeat=0, projectileDuration=0;

            //health and armor params
            static int initialHealth=100;

            //forming a section
            if (ImGui::CollapsingHeader("Transform params", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("Position X:", &positionX, 10);
                ImGui::InputInt("Position Y:", &positionY, 10);
                ImGui::SliderInt("Scale X:", &scaleX, 1, 10);
                ImGui::SliderInt("Scale Y:", &scaleY, 1, 10);
                ImGui::SliderAngle("Rotation (in Deg):", &rotationDeg, 0, 360);
            }
            ImGui::Spacing();
            //end of the section

            //forming a section
            if (ImGui::CollapsingHeader("Available sprites", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("Texture id", &selectedSpriteIdx, sprites, IM_ARRAYSIZE(sprites));
            }
            ImGui::Spacing();
            //end of the section
            
            //forming a section
            if (ImGui::CollapsingHeader("RigidBody params", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("Velocity X:", &velocityX);
                ImGui::InputInt("Velocity Y:", &velocityY);
            }
            ImGui::Spacing();
            //end of the section

            //forming a section
            if (ImGui::CollapsingHeader("Projectile params", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("Projectile Speed (pixels/sec):", &projectileSpeed, 10, 500);
                ImGui::SliderAngle("Proj. emitted angle (in Deg):", &projectileAngle, 0, 360);
                ImGui::InputInt("Projectile repeat rate:", &projectileRepeat, 1000);
                ImGui::InputInt("Projectile duration (frames):", &projectileDuration, 1000);
            }
            ImGui::Spacing();
            //end of the section

            //forming a section
            if (ImGui::CollapsingHeader("Health params", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("Initial player's health:", &initialHealth, 0, 100);
            }
            ImGui::Spacing();
            ImGui::Separator(); 
            ImGui::Spacing();


            //end of the section

            if (ImGui::Button("Spawn the enemy")) {
                //Here goes any processing of the event
                Entity npc = registry.CreateEntity();
                npc.Group("enemies");
                npc.AddComponent<TransformComponent>(glm::vec2(positionX, positionY), glm::vec2(scaleX, scaleY), glm::degrees(rotationDeg));
                npc.AddComponent<RigidBodyComponent>(glm::vec2(velocityX, velocityY));
                //hardcoded z-index for npc = 2 
                npc.AddComponent<SpriteComponent>(sprites[selectedSpriteIdx], 2, false, Game::mapInfo.tileSize, Game::mapInfo.tileSize);
                npc.AddComponent<BoxColliderComponent>(Game::mapInfo.tileSize, Game::mapInfo.tileSize);
                glm::vec2 projectileProjectedSpeed = {
                    projectileSpeed * cos(projectileAngle),
                    projectileSpeed * sin(projectileAngle),
                };
                npc.AddComponent<ProjectileEmitterComponent>(projectileProjectedSpeed, projectileRepeat * 1000, projectileDuration * 1000);
                npc.AddComponent<HealthComponent>(initialHealth);

                //resetting to defaults
                positionX = positionY = 0;
                scaleX = scaleY = 1;
                rotationDeg = 0;
                velocityX = velocityY = 0;
                projectileSpeed = projectileAngle = 0;
                projectileRepeat = projectileDuration = 0;
                initialHealth = 100;
            }
        }
        //closing the window
        ImGui::End();

        //a window that allows to track mouse movement
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);

        if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
            auto mouseInputPos = ImGui::GetIO().MousePos;
            ImGui::Text(
                "Map coordinates: (x=%0.2f, y=%0.2f)",
                camera.x + mouseInputPos.x,
                camera.y + mouseInputPos.y
            );
        }
        ImGui::End();


        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};



#endif