/**
 * @file UISystem.hpp
 * @brief Manages the mapping of UIScreens to these hardware layers.
 *
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "core/enums.h"
#include "core/routerIDs.hpp"
#include "events/UIEvents.hpp"
#include "soundbank.h"
#include <aegis/system.hpp>

#include "events/GenericEvents.hpp"

#include "components/menu/UIMenu.h"
#include "components/ui/UIScreen.h"
#include "controllers/MusicController.h"
#include "managers/RenderManager.hpp"

// TODO: add a way to indicate reduced # of bg slots
class UISystem : public ae::SystemRouter<UISystem,
                                         Event::ConfigureUIScreen,
                                         Event::ConfigureUIMenu,
                                         Event::ShowMenu,
                                         Event::HideAllMenus,
                                         Event::ShowScreen,
                                         Event::HideAllScreens,
                                         Event::SwitchView>,
                 public ae::Singleton<UISystem>
{
  public:
    void Init() override;

    /**
     * @brief Unloads and cleans up all registered screens. Wrapper for cleanup
     */
    void Shutdown() override;

    void Update(ae::fixed_t /*dt*/) override;

    // TODO: move out of UISystem. Only here as a temporary fix
    void on_receive(const Event::SwitchView& msg);

    /**
     * @brief ETL message handler to configure UIScreens
     *
     * @details First, it resets any previous configs via cleanupScreens. Second, it sets
     * the background pointers to render screens to. The arrays passed into
     * setGraphics() must contain the actual libnds hardware background layer IDs
     * (e.g., 0, 1, 2, 3). Third, it register screens via registerScreen to have them
     * pre-loaded before calling show.
     *
     * @note Required to call in order to use UIScreens
     *
     * @param config The event payload containing the UIScreen configuration to apply.
     */
    void on_receive(const Event::ConfigureUIScreen& config);

    /**
     * @brief ETL message handler to configure UIMenus
     *
     * @details First, it resets any previous configs via cleanupMenus. Second, it sets
     * all menus to be deactivated, and sets their text component pointer.
     *
     * @note Required to call in order to enable & use UIMenus
     *
     * @param config The event payload containing the UIMenu configuration to apply.
     */
    void on_receive(const Event::ConfigureUIMenu& config);

    /**
     * @brief ETL message handler to switch to the specified menu
     *
     * @details If another menu  is already loaded & hidden, it deactivates that menu and activates
     * the supplied menu pointer instead
     *
     * @note Cannot load multiple menus at one time.
     *
     * @param msg The event payload containing the UIMenu to switch to.
     */
    void on_receive(const Event::ShowMenu& msg);

    /**
     * @brief ETL message handler to hide all menus
     *
     * @param msg The event trigger.
     */
    void on_receive(const Event::HideAllMenus& /*msg*/);

    /**
     * @brief ETL message handler to switch to the specified screen
     *
     * @details If the screen is already loaded & hidden, it displays screen. If the screen
     * is not loaded, it loads and displays the screen.
     *
     * @note Cannot load the same UIScreen on both sub and main. Cannot load multiple
     * UIScreens on the same physical screen (main or sub)
     *
     * @param msg The event payload containing the UIScreen to switch to.
     */
    void on_receive(const Event::ShowScreen& msg);

    /**
     * @brief ETL message handler to hides all screens.
     *
     * Wrapper for hideAllScreens
     */
    void on_receive(const Event::HideAllScreens& /*msg*/);

    /**
     * @brief Fallback handler for unhandled ETL messages.
     *
     * @details Required by the ETL message router interface. Safely ignores
     * any messages routed to the UISystem that do not have a specific handler.
     *
     * @param msg The unhandled incoming message (unused).
     */
    void on_receive_unknown(const etl::imessage& msg)
    {
    }

  private:
    friend class Singleton<UISystem>;
    UISystem() : SystemRouter(kUISystemRouterID)
    {
    }

    /**
     * @brief Updates the order of lruBgSub/lruBgMain for the "least recently updated" id
     *
     * @details The lruBgMain/lruBgSub arrays dynamically shuffle to track the Least Recently Used
     * (LRU) hardware layer at index [0], ensuring the oldest visible screen is
     * the one overwritten when capacity is reached.
     *
     * @param id the background id
     * @param isMain flag indicating if the screen is to be rendered on the main (or sub) screen
     */
    void lruUpdate(int id, bool isMain);

    /**
     * @brief Registers screens to UISystem to have them pre-loaded before calling show
     *
     * @details When a screen is registered, it is assigned an available hardware bgId. Persistent
     * = is always loaded into memory. Paged (swappable) = can be loaded/unloaded into
     * memory.
     *
     * @param screen the screen to register
     */
    void registerScreen(UIScreen* screen);

    /**
     * @brief hides all screens.
     */
    void hideAllScreens();

    /**
     * @brief Unloads and cleans up all registered screens. Used to reset previous configs
     */
    void cleanupScreens();

    /**
     * @brief Unloads and cleans up all registered menus. Used to reset previous configs
     */
    void cleanupMenus();

    /**
     * @brief Stops playing the current sound effect, if any.
     */
    void cancelSFX();

    RenderManager& render = RenderManager::GetInstance();

    OamState* oamSub = nullptr;
    OamState* oamMain = nullptr;

    /// background ids. The order of the arrays matter. Front = least recently updated, back = last updated
    std::array<int, 4> lruBgSub = {0, 0, 0, 0};
    std::array<int, 3> lruBgMain = {0, 0, 0};

    /// original background ids (order doesn't change)
    std::array<int, 4> hwBgSub = {0, 0, 0, 0};
    std::array<int, 3> hwBgMain = {0, 0, 0};

    /// currently loaded screens (max 4 sub, 3 main)
    int screenMainCount = 0;
    int screenSubCount = 0;
    std::array<UIScreen*, 4> loadedSub{nullptr, nullptr, nullptr, nullptr};
    std::array<UIScreen*, 3> loadedMain = {nullptr, nullptr, nullptr};

    // menu
    std::array<UIMenu*, 10> menus = {};
    UIMenu* activeMenu = nullptr;
    TextComponent* text = nullptr;
    MusicController* musicCtrl = MusicController::getInstance();

    // menu sfx
    mm_sfxhand sfxMenuHandle;
    mm_sfxhand sfxSelectHandle;
    mm_sfxhand sfxCancelHandle;
};
