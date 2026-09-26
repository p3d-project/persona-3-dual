/**
 * @file UIManager.hpp
 * @brief Manager for hardware specific rendering functions related to the UI
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <aegis/manager.hpp>

class UIManager : public ae::Manager, public ae::Singleton<UIManager>
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
     * @brief Shows the specified background
     *
     * @param bgId background layer to show
     */
    void showBg(int bgId);

    /**
     * @brief Hides the specified background
     *
     * @param bgId background layer to hide
     */
    void hideBg(int bgId);

  private:
    friend class Singleton<UIManager>;
    UIManager() = default;
};
