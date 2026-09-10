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

    void initialize3DView(View3DConfig config);

    void cleanup3DView();

    void uploadTexture(int& textureID,
                       const GL_TEXTURE_TYPE_ENUM texType,
                       const int sizeX,
                       const int sizeY,
                       int param,
                       const void* bitmap);

    void renderTexturedModel(const void* displayList, const int texture);

    void renderTexturedBillboard(
        BillboardData bb, const int texture, bool faceCamera, ae::q20_12_t camX, ae::q20_12_t camY, ae::q20_12_t camZ);

    void renderDisplayList(const void* list);

    void deleteTexture(int& texture);

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;

    MathManager& math = MathManager::GetInstance();
};
