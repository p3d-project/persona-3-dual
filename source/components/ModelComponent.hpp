/**
 * @file ModelComponent.hpp
 * @brief Orchestrates rendering models from a 3D model file.
 *
 * @author Gregory Munro (ggmini)
 */

#pragma once
#include "components/AnimatorComponent.hpp"
#include "managers/RenderManager.hpp"
#include "types/aeTypes.hpp"

#include <aegis/component.hpp>
#include <memory>
#include <string>

class ModelComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Model);

    void Init() override
    {
    }

    /**
     * @brief Sets isActive to false on component destruction
     */
    void Destroy() override;

    /**
     * @brief Draws the mesh to the screen every frame.
     */
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

    RenderManager& render = RenderManager::GetInstance();

    /**
     * @brief Loads a texture header from the file buffer into memory.
     * @param buffer The file buffer to read from.
     * @param tex The texture object to populate with the header data.
     * @return true if the header was successfully loaded, false otherwise.
     */
    bool loadTextureHeader(FILE* f, MDL3Texture& tex);
    /**
     * @brief Loads an embedded texture from the file buffer and uploads it to the GPU.
     * @param buffer The file buffer to read from.
     * @param tex The texture object to populate with the texture data.
     * @return true if the texture was successfully loaded and uploaded, false otherwise.
     *
     * @note The function allocates a 32-bit aligned buffer for the GPU upload and frees it after use.
     */
    bool loadTexture(FILE* f, MDL3Texture& tex);
};
