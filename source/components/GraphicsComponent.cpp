#include "GraphicsComponent.hpp"
#include "core/globals.h"
#include "data/spriteDb.h"

void GraphicsComponent::Destroy()
{
    unloadAll();
}

GraphicAsset GraphicsComponent::loadGraphic(const std::string& path)
{
    GraphicAsset asset;

    asset.id = ++id;
    asset.tiles = io.loadToRAM(io.getAssetFilePath(path, ".img.bin"), &asset.tilesLen);
    asset.pal = io.loadToRAM(io.getAssetFilePath(path, ".pal.bin"), &asset.palLen);
    asset.map = io.loadToRAM(io.getAssetFilePath(path, ".map.bin"), &asset.mapLen);

    loadedGraphics.push_back(asset);

    return asset;
}

GraphicAsset GraphicsComponent::loadSpriteGraphicImpl(const std::string& spritePath, SpriteType type, int spriteId)
{
    std::string filename = getSpriteFilename(type, spriteId);
    if (filename.empty())
    {
        return GraphicAsset{};
    }

    return loadGraphic(spritePath + filename + "/" + filename);
}

void GraphicsComponent::unloadGraphic(GraphicAsset& asset)
{
    io.unloadFromRAM(asset.tiles);
    io.unloadFromRAM(asset.pal);
    io.unloadFromRAM(asset.map);

    int tempId = asset.id;

    asset.id = -1;
    asset.tiles = NULL;
    asset.tilesLen = 0;
    asset.pal = NULL;
    asset.palLen = 0;
    asset.map = NULL;
    asset.mapLen = 0;

    loadedGraphics.erase(loadedGraphics.begin() + tempId);
}

void GraphicsComponent::unloadAll()
{
    for (size_t i = 0; i < loadedGraphics.size(); ++i)
    {
        unloadGraphic(loadedGraphics[i]);
    }
    loadedGraphics.clear();
}
