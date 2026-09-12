/**
 * @file RenderManager.hpp
 * @brief Manager for hardware specific rendering functionse
 * @author Gregory Munro (ggmini)
 */

#pragma once
#include "managers/MathManager.hpp"
#include "types/EnvironmentTypes.hpp"
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
     */
    void uploadTexture(int& textureID,
                       const GL_TEXTURE_TYPE_ENUM texType,
                       const int sizeX,
                       const int sizeY,
                       int param,
                       const void* bitmap);

    /**
     * @brief Renders a textured model.
     *
     * @param displayList Pointer to the display list of the model to render.
     * @param texture The ID of the texture to use for rendering.
     */
    void renderTexturedModel(const void* displayList, const int texture);

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

    /**
     * @brief Directly passes a display list to the GPU for rendering.
     * @param list Pointer to the display list to render.
     */
    void renderDisplayList(const void* list);

    /**
     * @brief Deletes a texture from the GPU and resets its ID.
     * @param texture The ID of the texture to delete. This will be set to 0 after deletion.
     */
    void deleteTexture(int& texture);

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;

    static inline const ae::q20_12_t DEGREE_MODIFIER = ae::q20_12_t{1 << 15} / ae::q20_12_t{360};

    MathManager& math = MathManager::GetInstance();
};
