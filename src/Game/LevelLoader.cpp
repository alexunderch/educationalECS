#include "LevelLoader.h"
#include "Game.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ScriptComponent.h"


#include <fstream>
#include <sstream>
#include <sol/sol.hpp>

LevelLoader::LevelLoader() {
    Logger::Log("Level loader contructor was called!");    
}

LevelLoader::~LevelLoader() {
    Logger::Log("Level loader destructor was called!");    
}



void LevelLoader::LoadLevel(sol::state& lua, Registry& registry, AssetStore& assetStore, SDL_Renderer* renderer, int levelNumber) {
    std::string filePath = "./assets/scripts/Level" + std::to_string(levelNumber) + ".lua";
    sol::load_result script = lua.load_file(filePath);

    //checking the syntax but not executing the script
    if (!script.valid()) {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Error("Error while loading the script " + errorMessage);
        return;
    }

    //executing the script, using the Sol state
    lua.script_file(filePath);

    sol::table level = lua["Level"];

    int i = 0;
    sol::table assets = level["assets"];
    while (true) {
        //Looping across all of the assets
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        } 
        sol::table asset = assets[i];
        std::string assetType = asset["type"];

        if (assetType == "texture") {
            assetStore.AddTexture(renderer, asset["id"], asset["file"]);
        } else if (assetType == "font"){
            assetStore.AddFont(asset["id"], asset["file"], asset["font_size"]);
        }
        i++;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Read the level tilemap information
    ////////////////////////////////////////////////////////////////////////////
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    Logger::Warning(mapTextureAssetId);
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    double mapScale = map["scale"];

    //creating a file reader (stream)
    std::fstream mapFile(mapFilePath);

    Game::mapInfo = MapInfo(mapNumRows, mapNumCols, tileSize, mapScale);
    auto tilescaleY = (float) Game::windowHeight / (Game::mapInfo.numColumns * Game::mapInfo.tileSize);
    auto tilescaleX = (float) Game::windowWidth / (Game::mapInfo.numRows * Game::mapInfo.tileSize);

    Game::mapInfo.tileScale = static_cast<int>(tilescaleX / tilescaleY + 1);

    if (!mapFile.is_open()) {
        Logger::Error("Can't read the tile map!");
    }

    //buffer (one line)
    std::string line;
    int row_ind = 0;

    // Read all lines of the source comma separated file
    while (std::getline(mapFile, line)) {
        
        int col_ind = 0;
        // Now we read a complete line into our std::string line
        // Put it into a std::istringstream to be able to extract it with iostream functions
        std::stringstream iss(line);
        
        //we read the line as a stream
        std::string substring;
        // Now, in a loop, get the substrings from the std::istringstream
        while (std::getline(iss, substring, ',')) {
            
            int tileIdX = static_cast<int>(substring[1] - '0');
            int tileIdY = static_cast<int>(substring[0] - '0');

            int srcRectX = Game::mapInfo.tileSize * tileIdX;
            int srcRectY = Game::mapInfo.tileSize * tileIdY;

            Entity tile = registry.CreateEntity();
            tile.AddComponent<TransformComponent>(
                glm::vec2(
                    col_ind * (Game::mapInfo.tileScale * Game::mapInfo.tileSize),
                    row_ind * (Game::mapInfo.tileScale * Game::mapInfo.tileSize)
                ), 
                glm::vec2(Game::mapInfo.tileScale, Game::mapInfo.tileScale), 
                0.0
            );

            tile.AddComponent<SpriteComponent>(
                mapTextureAssetId, 
                0,
                false,
                Game::mapInfo.tileSize,
                Game::mapInfo.tileSize,
                srcRectX,
                srcRectY
            );
            tile.Group("tiles");

            col_ind++;
        }

        if (col_ind > Game::mapInfo.numColumns) {
            Logger::Error("Number of columns's been exceeded!");
        }
        row_ind++;
    }

    if (row_ind > Game::mapInfo.numRows) {
        Logger::Error("Number of columns's been exceeded!");
    }

    //close one in the end
    mapFile.close();

    // auto mapWidth = mapNumCols * tileSize * mapScale;
    // auto mapHeight = mapNumRows * tileSize * mapScale;

    ////////////////////////////////////////////////////////////////////////////
    // Read the level entities and their components
    ////////////////////////////////////////////////////////////////////////////
    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];
        Entity newEntity = registry.CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false),
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }
            
            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }
            
            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlledComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }

            // Custom scripting component
            sol::optional<sol::table> script = entity["components"]["on_update_script"];
            if (script != sol::nullopt) {
                sol::function func = entity["components"]["on_update_script"][0];
                newEntity.AddComponent<ScriptComponent>(func);
            }
        }   
        i++;
    }

    // //Text label on the screen 
    // Entity label = registry->CreateEntity();
    // label.AddComponent<TextLabelComponent>(glm::vec2(500, 20), "My 2D-ass Game", "charriot-font", SDL_Color{255, 255, 255, 255}, true);

}