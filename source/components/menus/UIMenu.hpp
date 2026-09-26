#pragma once

#define MENU_BIND(ClassName, Method) reinterpret_cast<ViewState (UIMenu::*)()>(&ClassName::Method)

#include <etl/span.h>
#include <etl/stack.h>
#include <maxmod9.h>
#include <nds.h>
#include <string>

#include "components/TextComponent.hpp"
#include "types/UITypes.hpp"

class UIMenu
{
  protected:
    std::string pauseMessage = "Pause";
    bool isActive = false;
    TextComponent* text = nullptr;

    // options
    etl::span<MenuOption> options;
    int selectedOption = 0;
    int startIndex = 0;

    /**
     * @brief Changes the active menu.
     *
     * @param newOptions the new menu options.
     * @return ViewState the View to switch to
     */
    ViewState changeMenu(etl::span<MenuOption> newOptions);

  private:
    friend class UISystem;

    // options
    /// @note visibleOptions is set via the UISystem via the ConfigureUIMenu event handler
    int visibleOptions = 0;
    etl::stack<MenuState, 5> prevOptions;
    ViewState nextViewState = ViewState::KEEP_CURRENT;

    /**
     * @brief Changes the current options to the previous options
     *
     * @note Override if a menu manually manages its state (doesn't use changeMenu, default prevOption functions)
     */
    virtual void prevOption();

    /**
     * @brief Used to set the default pauseMessage and options values
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
