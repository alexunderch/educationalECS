#include "AssetStore.h"
#include "../Logger/Logger.h"
AssetStore::AssetStore(){
    Logger::Log("Called the AssetStore contructor!");
}

AssetStore::~AssetStore(){
    ClearAssets();
    Logger::Log("Called the AssetStore destructor!");
}

std::vector<std::string> AssetStore::ListTextures() const {
    std::vector<std::string> values;
    for (auto [texture_id, _]: textures) {
        if (texture_id.size()) {
            values.push_back(texture_id);
        }
    }
    return values;
}

void AssetStore::ClearAssets(){
    for (auto [key, value]: textures){
        SDL_DestroyTexture(value);
    }

    for (auto [key, value]: fonts){
        TTF_CloseFont(value);
    }
    
    textures.clear();
    fonts.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath){
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    //Adding texture to the map
    //https://stackoverflow.com/questions/17172080/insert-vs-emplace-vs-operator-in-c-map
    textures.emplace(assetId, texture);
    Logger::Log("New texture added to the Asset Store with id = " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) const {
    if (textures.find(assetId) == textures.end()){
        throw std::out_of_range("No such id for the texture: " + assetId);
    }
    return textures.at(assetId); // at = [] const
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filePath, size_t fontSize) {
    fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));
    Logger::Log("New font added to the Asset Store with id = " + assetId);
}

TTF_Font* AssetStore::GetFont(const std::string& assetId) const {
    if (fonts.find(assetId) == fonts.end()){
        throw std::out_of_range("No such id for the font: " + assetId);
    }
    return fonts.at(assetId);
}
