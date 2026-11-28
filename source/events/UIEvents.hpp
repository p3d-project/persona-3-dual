/**
 * @file UIEvents.hpp
 * @brief Events for UISystem
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "components/TextComponent.hpp"
#include "components/menus/UIMenu.hpp"
#include "components/screens/UIScreen.hpp"
#include "types/aeTypes.hpp"

#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief All parameters needed to configure UIScreens
 */
struct ConfigureUIScreen : public etl::message<EventID::CONFIGURE_UI_SCREEN>
{
    /// Array of sub screen background ids
    std::array<int, 3> bgSub;

    /// Array of main screen background ids
    std::array<int, 2> bgMain;

    /// Pointer to sub OAM
    OamState* oamSub;

    /// Pointer to main OAM
    OamState* oamMain;

    /// The screens to register
    std::array<UIScreen*, 5> screens;

    ConfigureUIScreen(std::array<int, 3> iBgSub,
                      std::array<int, 2> iBgMain,
                      OamState* iOamSub,
                      OamState* iOamMain,
                      std::array<UIScreen*, 5> iScreens)
        : bgSub(iBgSub), bgMain(iBgMain), oamSub(iOamSub), oamMain(iOamMain), screens(iScreens)
    {
    }
};

/**
 * @brief All parameters needed to configure UIMenus
 */
struct ConfigureUIMenu : public etl::message<EventID::CONFIGURE_UI_MENU>
{
    TextComponent* text = nullptr;
    std::array<UIMenu*, 10> menus = {};

    ConfigureUIMenu(TextComponent* iText, std::array<UIMenu*, 10> iMenus) : text(iText), menus(iMenus)
    {
    }
};

/**
 * @brief Event to show specified menu
 */
struct ShowMenu : public etl::message<EventID::SHOW_MENU>
{
    UIMenu* menu;
    ShowMenu(UIMenu* iMenu) : menu(iMenu)
    {
    }
};

/**
 * @brief Event to hide all menus
 */
struct HideAllMenus : public etl::message<EventID::HIDE_ALL_MENUS>
{
};

/**
 * @brief Event to show specified screen
 */
struct ShowScreen : public etl::message<EventID::SHOW_SCREEN>
{
    /// The screen to show
    UIScreen* screen;

    ShowScreen(UIScreen* iScreen) : screen(iScreen)
    {
    }
};

/**
 * @brief Event to trigger hiding all UIScreens
 */
struct HideAllScreens : public etl::message<EventID::HIDE_ALL_SCREENS>
{
};

/**
 * @brief Event to trigger rendering UI text
 */
struct RenderUIText : public etl::message<EventID::RENDER_UI_TEXT>
{
};
} // namespace Event
