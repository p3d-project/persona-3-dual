#include "MainMenuComponent.h"
#include "core/globals.h"
#include "events/GenericEvents.hpp"
#include "events/SaveEvents.hpp"
#include <string>

MainMenuComponent* MainMenuComponent::instance = nullptr;

void MainMenuComponent::create()
{
    if (instance == nullptr)
    {
        instance = new MainMenuComponent();
    }
}

void MainMenuComponent::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

MainMenuComponent* MainMenuComponent::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

void MainMenuComponent::resetHook()
{
    pauseMessage = "";
    options = mainMenuOptions;
    optionCount = MAIN_MENU_OPTIONS;
}

void MainMenuComponent::closeHook()
{
    resetMenu();
    ae::BroadcastEvent(Event::SwitchView{ViewState::INTRO});
}

// option handlers
ViewState MainMenuComponent::mainMenuOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<MainMenuOptions>(selectedOption))
    {
    case MainMenuOptions::LOAD_GAME:
        changeMenu(levelOptions, LEVEL_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case MainMenuOptions::SETTINGS:
        changeMenu(settingOptions, SETTING_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case MainMenuOptions::RETURN_TO_TITLE:
        selectedView = ViewState::INTRO;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::levelOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<LevelOptions>(selectedOption))
    {
    case LevelOptions::START_GAME:
        selectedView = ViewState::CUTSCENE_1;
        break;
    case LevelOptions::IWATODAI_DORM:
        selectedView = ViewState::IWATODAI_DORM;
        break;
    case LevelOptions::IWATODAI_STREETS:
        selectedView = ViewState::IWATODAI_STREETS;
        break;
    case LevelOptions::STATION:
        selectedView = ViewState::STATION;
        break;
    case LevelOptions::PAULOWNIA_MALL:
        selectedView = ViewState::PAULOWNIA_MALL;
        break;
    case LevelOptions::SIGN_CONTRACT:
        selectedView = ViewState::SIGN_CONTRACT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::settingOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<SettingOptions>(selectedOption))
    {
    case SettingOptions::CHANGE_INTRO_VIDEO:
        changeMenu(settingIntroOptions, SETTING_INTRO_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case SettingOptions::FEMC_MODE:
        saveData.femcMode = !saveData.femcMode;
        updateSave();
        selectedView = ViewState::KEEP_CURRENT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::settingIntroOptionSelected()
{
    switch (static_cast<SettingIntroOptions>(selectedOption))
    {
    case SettingIntroOptions::ORIGINAL:
        strncpy(saveData.introVideoPath, "base.vid", sizeof(saveData.introVideoPath));
        break;
    case SettingIntroOptions::FES:
        strncpy(saveData.introVideoPath, "fes.vid", sizeof(saveData.introVideoPath));
        break;
    case SettingIntroOptions::PORTABLE:
        strncpy(saveData.introVideoPath, "portable.vid", sizeof(saveData.introVideoPath));
        break;
    case SettingIntroOptions::RELOAD:
        strncpy(saveData.introVideoPath, "reload.vid", sizeof(saveData.introVideoPath));
        break;
    default:
        strncpy(saveData.introVideoPath, "reload.vid", sizeof(saveData.introVideoPath));
    }

    updateSave();
    return ViewState::KEEP_CURRENT;
}

void MainMenuComponent::updateSave()
{
    ae::BroadcastEvent(Event::WriteSave{});
}
