/**
 * @file RenderManager.hpp
 * @brief Manager for hardware specific rendering functionse
 * @author Gregory Munro (ggmini)
 */

#pragma once
#include "managers/MathManager.hpp"
#include "types/EnvironmentTypes.hpp"
#include "types/MeshTypes.hpp"
#include "types/RenderTypes.hpp"
#include <aegis/manager.hpp>

class RenderManager : public ae::Manager, public ae::Singleton<RenderManager>
{
  public:
    void Init() override
    {
    }

    void Process() override
    {
    }

    void Shutdown() override
    {
    }

    /**
     * @brief Initialize a new 3D view.
     * @param config The configuration for the 3D view.
     *
     * @details This function initializes the 3D view with the specified configuration.
     * It does all of the general configuration, and also activates Fog and Outlines if they are present in the configuration.
     *
     * @note This function will check config.outlineColor and config.fogRed respectively to determine if outlins or fog need to be enabled.
     * If these values are -1, then that feature will not be enabled. The general setting for these features is not affected.
     * This means if these values are -1, GL_FOG or GL_OUTLINE should not be activated in config.settings, else they will work of default data.
     * In turn GL_FOG or GL_OUTLINE will need to be activated in config.settings if they are desired, as this is not handled automatically.
     */
    void initialize3DView(View3DConfig config);

    /**
     * @brief Cleans up the 3D view, resetting the state.
     */
    void cleanup3DView();

    /**
     * @brief Uploads a texture to the GPU.
     *
     * @param textureID The ID of the texture to upload.
     * @param texType The type of the texture to upload (i.e. RGB16, RGBA, ...).
     * @param sizeX The width of the texture in pixels.
     * @param sizeY The height of the texture in pixels.
     * @param param The parameters of the texture.
     * @param bitmap Pointer to the texture data to load.
     * @return True if the texture was successfully uploaded, false otherwise.
     *
     * @note Will delete old texture if textureID is not -1, and will set textureID to the new texture ID.
     */
    bool uploadTexture(int& textureID,
                       const GL_TEXTURE_TYPE_ENUM texType,
                       const int sizeX,
                       const int sizeY,
                       int param,
                       const void* bitmap);

    /**
     * @brief Renders a MDL3 model.
     *
     * @param model The 3D model to render.
     *
     * @note This function will render the model at the origin (0, 0, 0) with no rotation or scaling applied.
     * It is intended for environment models. There is a separate function for rendering with transformations.
     */
    void renderMeshComponent(MDL3Model& model);

    /**
     * @brief Renders a MDL3 model.
     *
     * @param model The 3D model to render.
     * @param posX The X position of the model.
     * @param posY The Y position of the model.
     * @param posZ The Z position of the model.
     * @param rotX The X rotation of the model.
     * @param rotY The Y rotation of the model.
     * @param rotZ The Z rotation of the model.
     * @param scale The scale of the model (default 1).
     */
    void renderMeshComponent(MDL3Model& model,
                             ae::q20_12_t posX,
                             ae::q20_12_t posY,
                             ae::q20_12_t posZ,
                             uint32_t rotX,
                             uint32_t rotY,
                             uint32_t rotZ,
                             ae::q20_12_t scale = ae::q20_12_t{1});

    /**
     * @brief Renders a textured billboard.
     *
     * @param bb The billboard data to render.
     * @param texture The ID of the texture to use for rendering.
     * @param faceCamera Whether the billboard should face the camera.
     * @param camX The X position of the camera.
     * @param camY The Y position of the camera.
     * @param camZ The Z position of the camera.
     */
    void renderTexturedBillboard(
        BillboardData bb, const int texture, bool faceCamera, ae::q20_12_t camX, ae::q20_12_t camY, ae::q20_12_t camZ);

    // TODO: make this function inline
    /**
     * @brief Renders a textured model.
     *
     * @param displayList Pointer to the display list of the model to render.
     * @param texture The ID of the texture to use for rendering.
     *
     * @warning This function should not be used outside of RenderManager as it will soon be inlined & private.
     * Calls from outside of RenderManager should start using renderMeshComponent() instead.
     */
    void renderDisplayList(const void* displayList, const int texture);

    /**
     * @brief Deletes a texture from the GPU and resets its ID.
     * @param texture The ID of the texture to delete. This will be set to -1 after deletion.
     */
    void deleteTexture(int& texture);

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;

    static inline const ae::q20_12_t DEGREE_MODIFIER = ae::q20_12_t{1 << 15} / ae::q20_12_t{360};
    int activeTexture = -1;

    MathManager& math = MathManager::GetInstance();
};
