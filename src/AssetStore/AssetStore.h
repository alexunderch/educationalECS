#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>



class AssetStore{
private:
    std::map<std::string, SDL_Texture*> textures;
    //TODO: fonts, audio data...
    std::map<std::string, TTF_Font*> fonts;


public:
    AssetStore();
    ~AssetStore();

    void ClearAssets();
    void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath);
    void AddFont(const std::string& assetId, const std::string& filePath, size_t fontSize);
    SDL_Texture* GetTexture(const std::string& assetId) const;
    TTF_Font* GetFont(const std::string& assetId) const;

    std::vector<std::string> ListTextures() const;

};




#endif