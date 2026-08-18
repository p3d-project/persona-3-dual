/**
 * @file UIEvents.hpp
 * @brief Events for UISystem
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "components/ui/UIScreen.h"
#include "core/enums.h"
#include "core/structs.h"
#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief All parameters needed to configure the UISystem in one call.
 */
struct ConfigureUIScreen : public etl::message<EventID::ConfigureUIScreen>
{
    /// Array of sub screen background ids
    std::array<int, 4> bgSub;

    /// Array of main screen background ids
    std::array<int, 3> bgMain;

    /// Pointer to sub OAM
    OamState* oamSub;

    /// Pointer to main OAM
    OamState* oamMain;

    /// The screens to register
    std::array<UIScreen*, 7> screens;

    ConfigureUIScreen(std::array<int, 4> iBgSub,
                      std::array<int, 3> iBgMain,
                      OamState* iOamSub,
                      OamState* iOamMain,
                      std::array<UIScreen*, 7> iScreens)
        : bgSub(iBgSub), bgMain(iBgMain), oamSub(iOamSub), oamMain(iOamMain), screens(iScreens)
    {
    }
};

// TODO: add doxygen docs
struct ConfigureUIMenu : public etl::message<EventID::ConfigureUIMenu>
{
    TextComponent* text = nullptr;
    std::array<UIMenu*, 10> menus = {};

    ConfigureUIMenu(TextComponent* iText, std::array<UIMenu*, 10> iMenus) : text(iText), menus(iMenus)
    {
    }
};

// TODO: add doxygen docs
struct ShowMenu : public etl::message<EventID::ShowMenu>
{
    UIMenu* menu;
    ShowMenu(UIMenu* iMenu) : menu(iMenu)
    {
    }
};

// TODO: add doxygen docs
struct HideAllMenus : public etl::message<EventID::HideAllMenus>
{
};

/**
 * @brief Event to show specified screen
 */
struct ShowScreen : public etl::message<EventID::ShowScreen>
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
struct HideAllScreens : public etl::message<EventID::HideAllScreens>
{
};
} // namespace Event
