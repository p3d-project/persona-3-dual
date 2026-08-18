/**
 * @file GraphicsComponent.hpp
 * @brief Orchestrates graphic loading and management
 *
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "core/enums.h"
#include "core/structs.h"
#include "managers/IOManager.hpp"
#include <aegis/component.hpp>
#include <etl/vector.h>

class GraphicsComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Graphics);
    void Init() override
    {
    }

    void Destroy() override;

    void Update(ae::fixed_t /*dt*/) override
    {
    }

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Loads a graphic asset's tiles, pal, and map into memory
     *
     * @param path Path relative to the base path.
     * @return GraphicAsset with the loaded graphics data
     */
    GraphicAsset loadGraphic(const std::string& path);

    /**
     * @brief Wrapper function that loads a sprite graphic asset into memory
     *
     * @details This is a templated wrapper function, which calls loadSpriteGraphicImpl
     *
     * @tparam SpriteID
     * @param spritePath Path relative to the base path.
     * @param type Sprite type
     * @param spriteId Sprite id
     * @return GraphicAsset with the loaded graphics data
     */
    template <typename SpriteID>
    GraphicAsset loadSpriteGraphic(const std::string& spritePath, SpriteType type, SpriteID spriteId)
    {
        return loadSpriteGraphicImpl(spritePath, type, static_cast<int>(spriteId));
    }

    /**
     * @brief Unload a specific graphic asset from memory
     *
     * @param asset Asset to unload
     */
    void unloadGraphic(GraphicAsset& asset);

    /**
     * @brief Unload all graphic assets from memory
     */
    void unloadAll();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    IOManager& io = IOManager::GetInstance();

    /// @brief A GraphicAsset's index in loadedGraphics
    int id = -1;

    // TODO: set vector size to something appropriate
    etl::vector<GraphicAsset, 32> loadedGraphics;

    /**
     * @brief Loads the sprite graphic asset's tiles, pal, and map into memory via the spriteDb
     *
     * @param spritePath Path relative to the base path.
     * @param type Sprite type
     * @param spriteId Sprite id
     * @return GraphicAsset with the loaded graphics data
     */
    GraphicAsset loadSpriteGraphicImpl(const std::string& spritePath, SpriteType type, int spriteId);
};
