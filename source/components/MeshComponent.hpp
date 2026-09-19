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
#include <memory>
#include <string>

class MeshComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Mesh);

    void Init() override
    {
    }

    /**
     * @brief Sets isActive to false on component destruction
     */
    void Destroy() override;

    void Update(ae::q20_12_t /*dt*/) override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Loads a mesh from a file into memory.
     * @param meshFilePath The path to the mesh file to load.
     * @return true if the mesh was successfully loaded, false otherwise.
     */
    bool loadMesh(std::string* meshFilePath);

    /**
     * @brief Draws the loaded mesh to the screen.
     */
    void drawMesh();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    std::unique_ptr<MDL3Model> model;

    IOManager& io = IOManager::GetInstance();
    RenderManager& render = RenderManager::GetInstance();

    /**
     * @brief Loads a texture header from the file buffer into memory.
     * @param buffer The file buffer to read from.
     * @param tex The texture object to populate with the header data.
     * @param offset The current offset in the file buffer, which will be updated after reading.
     * @return true if the header was successfully loaded, false otherwise.
     */
    bool loadTextureHeader(FileBuffer& buffer, MDL3Texture& tex, size_t& offset);
    /**
     * @brief Loads an embedded image from the file buffer and uploads it to the GPU.
     * @param buffer The file buffer to read from.
     * @param tex The texture object to populate with the image data.
     * @param offset The current offset in the file buffer, which will be updated after reading.
     * @return true if the image was successfully loaded and uploaded, false otherwise.
     *
     * @note The function allocates a 32-bit aligned buffer for the GPU upload and frees it after use.
     */
    bool loadEmbeddedImage(FileBuffer& buffer, MDL3Texture& tex, size_t& offset);
};
