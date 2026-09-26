#include "GraphicsComponent.hpp"
#include "core/globals.hpp"
#include "data/spriteDb.hpp"

void GraphicsComponent::Destroy()
{
    unloadAll();
}

GraphicAsset GraphicsComponent::loadGraphic(const std::string& path)
{
    GraphicAsset asset;

    if (loadedGraphics.full())
    {
        return asset;
    }

    asset.id = ++id;
    asset.tiles = io.loadToRAM(io.getAssetFilePath(path, ".img.bin"), &asset.tilesLen);
    asset.pal = io.loadToRAM(io.getAssetFilePath(path, ".pal.bin"), &asset.palLen);
    asset.map = io.loadToRAM(io.getAssetFilePath(path, ".map.bin"), &asset.mapLen);

    if (asset.tiles == nullptr)
    {
        io.unloadFromRAM(asset.tiles);
        io.unloadFromRAM(asset.pal);
        io.unloadFromRAM(asset.map);
        return GraphicAsset{};
    }

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
    if (asset.id < 0)
    {
        return;
    }

    auto it = std::find_if(
        loadedGraphics.begin(), loadedGraphics.end(), [asset](GraphicAsset ga) { return ga.id == asset.id; });

    // asset was not found
    if (it == loadedGraphics.end())
    {
        return;
    }

    io.unloadFromRAM(asset.tiles);
    io.unloadFromRAM(asset.pal);
    io.unloadFromRAM(asset.map);

    asset.id = -1;
    asset.tiles = NULL;
    asset.tilesLen = 0;
    asset.pal = NULL;
    asset.palLen = 0;
    asset.map = NULL;
    asset.mapLen = 0;

    loadedGraphics.erase(it);
}

void GraphicsComponent::unloadAll()
{
    while (!loadedGraphics.empty())
    {
        unloadGraphic(loadedGraphics.back());
    }
}
