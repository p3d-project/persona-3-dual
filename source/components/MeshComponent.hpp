/**
 * @file MeshComponent.hpp
 * @brief Orchestrates rendering meshes from a bitmap.
 *
 * @author Gregory Munro (ggmini)
 */

#pragma once
#include "managers/IOManager.hpp"
#include "managers/RenderManager.hpp"
#include "types/MeshTypes.hpp"
#include "types/aeTypes.hpp"

#include <aegis/component.hpp>
#include <string>

class MeshComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Mesh);

    void Init() override
    {
    }

    void Destroy() override;

    void Update(ae::q20_12_t /*dt*/) override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    bool loadTextureHeader(FileBuffer& buffer, MDL3Texture& tex, size_t& offset);
    bool loadEmbeddedImage(FileBuffer& buffer, MDL3Texture& tex, size_t& offset);
    bool loadMesh(std::string* meshFilePath);

    void drawMesh();

  private:
    uint32_t nodeCount = 0;
    uint32_t texCount = 0;
    etl::vector<MDL3Texture, 8> textures;
    etl::vector<Node, 64> nodes;
    etl::vector<Animation, 32> animations;

    IOManager& io = IOManager::GetInstance();
    RenderManager& render = RenderManager::GetInstance();
};
