#pragma once

#define MENU_BIND(ClassName, Method) reinterpret_cast<ViewState (UIMenu::*)()>(&ClassName::Method)

#include "core/structs.h"
#include <maxmod9.h>
#include <nds.h>
#include <stack>
#include <string>

#include "components/TextComponent.hpp"

class UIMenu
{
  protected:
    std::string pauseMessage = "Pause";
    bool isActive = false;
    TextComponent* text = nullptr;

    // options
    MenuOption* options;
    int optionCount = 0;
    int selectedOption = 0;
    int startIndex = 0;

    /**
     * @brief Changes the active menu.
     *
     * @param newOptions the new menu options.
     * @param newOptionCount the number of new menu options.
     * @return ViewState the View to switch to
     */
    ViewState changeMenu(MenuOption* newOptions, int newOptionCount);

  private:
    friend class UISystem;

    // options
    const int visibleOptions = 23;
    std::stack<MenuState> prevOptions;
    ViewState nextViewState = ViewState::KEEP_CURRENT;

    /**
     * @brief Changes the current options to the previous options
     *
     * @note Override if a menu manually manages its state (doesn't use changeMenu, default prevOption functions)
     */
    virtual void prevOption();

    /**
     * @brief Used to set the default pauseMessage, options, and optionCount values
     */
    virtual void resetHook() = 0;

    /**
     * @brief Execute logic after the root menu is exited.
     */
    virtual void closeHook() = 0;

  public:
    /**
     * @brief Resets the menu to its initial state.
     */
    virtual void resetMenu();

    /**
     * @brief Optional hook to call during the update loop
     *
     * Can override the return from the standard update loop
     */
    virtual ViewState updateHook();
};
