#include "PauseMenu.hpp"
#include "core/globals.hpp"
#include "core/logger.hpp"
#include <nds.h>
#include <string>

PauseMenu* PauseMenu::instance = nullptr;

void PauseMenu::create()
{
    if (instance == nullptr)
    {
        instance = new PauseMenu();
    }
}

void PauseMenu::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

PauseMenu* PauseMenu::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

void PauseMenu::resetHook()
{
    logMessage(LogSeverity::DEBUG, "Pause menu opened");
    options = menuOptions;
    isClosed = false;
}

ViewState PauseMenu::updateHook()
{
    return ViewState::DEFAULT;
}

void PauseMenu::closeHook()
{
    logMessage(LogSeverity::DEBUG, "Pause menu closed");
    resetMenu();

    // set flag
    isClosed = true;
}

// menu navigation handlers

void PauseMenu::updateCameraOptionName()
{
    switch (cameraSystem.getMode())
    {
    case CameraMode::Follow:
        debugOptions[static_cast<int>(DebugOption::CYCLE_CAMERA_MODE)].name = "Camera: Follow";
        break;
    case CameraMode::Static:
        debugOptions[static_cast<int>(DebugOption::CYCLE_CAMERA_MODE)].name = "Camera: Static";
        break;
    case CameraMode::CCTV:
        debugOptions[static_cast<int>(DebugOption::CYCLE_CAMERA_MODE)].name = "Camera: CCTV";
        break;
    case CameraMode::Free:
        debugOptions[static_cast<int>(DebugOption::CYCLE_CAMERA_MODE)].name = "Camera: Free";
        break;
    case CameraMode::Path:
        debugOptions[static_cast<int>(DebugOption::CYCLE_CAMERA_MODE)].name = "Camera: Path";
        break;
    default:
        debugOptions[static_cast<int>(DebugOption::CYCLE_CAMERA_MODE)].name = "Camera: ?";
        break;
    }
}

ViewState PauseMenu::openDebugMenu()
{
    updateCameraOptionName();
    return changeMenu(debugOptions);
}

ViewState PauseMenu::openSkillMenu()
{
    return changeMenu(skillOptions);
}

ViewState PauseMenu::openItemMenu()
{
    return changeMenu(itemOptions);
}

ViewState PauseMenu::openPersonaMenu()
{
    return changeMenu(personaOptions);
}

ViewState PauseMenu::openEquipMenu()
{
    return changeMenu(equipOptions);
}

ViewState PauseMenu::openStatusMenu()
{
    return changeMenu(statsOptions);
}

ViewState PauseMenu::openSLinkMenu()
{
    return changeMenu(sLinkOptions);
}

ViewState PauseMenu::openSystemMenu()
{
    return changeMenu(systemOptions);
}

ViewState PauseMenu::openCharacterAnimMenu()
{
    return changeMenu(characterAnimOptions);
}

ViewState PauseMenu::openViewMenu()
{
    return changeMenu(viewOptions);
}

ViewState PauseMenu::openCutsceneMenu()
{
    return changeMenu(cutsceneOptions);
}

ViewState PauseMenu::openMusicMenu()
{
    return changeMenu(musicOptions);
}

ViewState PauseMenu::openBattleMusicMenu()
{
    return changeMenu(battleMusicOptions);
}

ViewState PauseMenu::openLocationMusicMenu()
{
    return changeMenu(locationMusicOptions);
}

ViewState PauseMenu::openMenuMusicMenu()
{
    return changeMenu(menuMusicOptions);
}

ViewState PauseMenu::openOtherMusicMenu()
{
    return changeMenu(otherMusicOptions);
}

// selection handlers

ViewState PauseMenu::skillOptionSelected()
{
    return changeMenu(skills);
}

ViewState PauseMenu::itemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenu::equipOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenu::personaOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenu::statsOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenu::sLinkOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenu::systemOptionSelected()
{
    switch (static_cast<SystemOption>(selectedOption))
    {
    case SystemOption::RETURN_TO_TITLE:
    {
        return ViewState::INTRO;
    }
    default:
    {
        return ViewState::KEEP_CURRENT;
    }
    }
}

ViewState PauseMenu::viewOptionSelected()
{
    switch (static_cast<ViewOption>(selectedOption))
    {
    case ViewOption::DISCLAIMER:
        return ViewState::DISCLAIMER;
    case ViewOption::INTRO:
        return ViewState::INTRO;
    case ViewOption::MAIN_MENU:
        return ViewState::MAIN_MENU;
    case ViewOption::IWATODAI_DORM:
        return ViewState::IWATODAI_DORM;
    case ViewOption::IWATODAI_STREETS:
        return ViewState::IWATODAI_STREETS;
    case ViewOption::STATION:
        return ViewState::STATION;
    case ViewOption::SIGN_CONTRACT:
        return ViewState::SIGN_CONTRACT;
    case ViewOption::PAULOWNIA_MALL:
        return ViewState::PAULOWNIA_MALL;
    default:
        return ViewState::KEEP_CURRENT;
    }
}

ViewState PauseMenu::cutsceneOptionSelected()
{
    switch (static_cast<CutsceneOption>(selectedOption))
    {
    case CutsceneOption::INTRO_VIDEO:
        return ViewState::INTRO_VIDEO;
    case CutsceneOption::CUTSCENE_1:
        return ViewState::CUTSCENE_1;
    case CutsceneOption::CUTSCENE_2:
        return ViewState::CUTSCENE_2;
    default:
        return ViewState::KEEP_CURRENT;
    }
}

ViewState PauseMenu::debugOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<DebugOption>(selectedOption))
    {
    case DebugOption::TOGGLE_BILLBOARDS:
        Globals::enableBillboards = !Globals::enableBillboards;
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case DebugOption::TOGGLE_DEBUG_PRINT:
        Globals::enableDebugPrint = !Globals::enableDebugPrint;
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case DebugOption::CYCLE_CAMERA_MODE:
        if (cameraSystem.getMode() == CameraMode::Path)
        {
            ae::BroadcastEvent(Event::SetCameraMode{CameraMode::Follow});
        }
        else
        {
            CameraMode mode = cameraModes[(static_cast<int>(cameraSystem.getMode()) + 1) % cameraModes.size()];
            ae::BroadcastEvent(Event::SetCameraMode{mode});
        }
        updateCameraOptionName();
        selectedView = ViewState::KEEP_CURRENT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }
    return selectedView;
}

ViewState PauseMenu::playMusicOptionSelected(AudioType type, int audioId)
{
    AudioManager& audio = AudioManager::GetInstance();
    AudioDBEntry entry = getAudioDBEntry(type, audioId);
    if (entry.type != AudioType::NONE)
    {
        audio.registerAudio(fatBasePath + "music/" + entry.filename + ".qoa", entry.loopStartTime, entry.loopEndTime);
        audio.playAudio();
    }
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenu::battleMusicOptionSelected()
{
    switch (static_cast<BattleAudio>(selectedOption))
    {
    case BattleAudio::MASS_DESTRUCTION:
        return playMusicOptionSelected(AudioType::BATTLE, static_cast<int>(BattleAudio::MASS_DESTRUCTION));
    default:
        return ViewState::KEEP_CURRENT;
    }
}

ViewState PauseMenu::locationMusicOptionSelected()
{
    switch (static_cast<LocationAudio>(selectedOption))
    {
    case LocationAudio::IWATODAI_DORM:
        return playMusicOptionSelected(AudioType::LOCATION, static_cast<int>(LocationAudio::IWATODAI_DORM));
    case LocationAudio::SUMMER_BOUND:
        return playMusicOptionSelected(AudioType::LOCATION, static_cast<int>(LocationAudio::SUMMER_BOUND));
    case LocationAudio::COLOR_YOUR_NIGHT:
        return playMusicOptionSelected(AudioType::LOCATION, static_cast<int>(LocationAudio::COLOR_YOUR_NIGHT));
    case LocationAudio::PAULOWNIA_MALL:
        return playMusicOptionSelected(AudioType::LOCATION, static_cast<int>(LocationAudio::PAULOWNIA_MALL));
    default:
        return ViewState::KEEP_CURRENT;
    }
}

ViewState PauseMenu::menuMusicOptionSelected()
{
    switch (static_cast<MenuAudio>(selectedOption))
    {
    case MenuAudio::MISTIC:
        return playMusicOptionSelected(AudioType::MENU, static_cast<int>(MenuAudio::MISTIC));
    case MenuAudio::TIGHTROPE:
        return playMusicOptionSelected(AudioType::MENU, static_cast<int>(MenuAudio::TIGHTROPE));
    case MenuAudio::ARIA_OF_THE_SOUL:
        return playMusicOptionSelected(AudioType::MENU, static_cast<int>(MenuAudio::ARIA_OF_THE_SOUL));
    default:
        return ViewState::KEEP_CURRENT;
    }
}

ViewState PauseMenu::otherMusicOptionSelected()
{
    switch (static_cast<OtherAudio>(selectedOption))
    {
    case OtherAudio::AFTER_THE_BATTLE:
        return playMusicOptionSelected(AudioType::OTHER, static_cast<int>(OtherAudio::AFTER_THE_BATTLE));
    case OtherAudio::BECAUSE_I_WILL_PROTECT_YOU:
        return playMusicOptionSelected(AudioType::OTHER, static_cast<int>(OtherAudio::BECAUSE_I_WILL_PROTECT_YOU));
    case OtherAudio::CLUB_ESCAPADE_NOUVEAU_METALMOTH_REMIX:
        return playMusicOptionSelected(AudioType::OTHER,
                                       static_cast<int>(OtherAudio::CLUB_ESCAPADE_NOUVEAU_METALMOTH_REMIX));
    default:
        return ViewState::KEEP_CURRENT;
    }
}

ViewState PauseMenu::characterAnimOptionSelected()
{
    animationCtrl->stop();

    ViewState selectedView = ViewState::KEEP_CURRENT;
    switch (static_cast<CharacterAnimOption>(selectedOption))
    {
    case CharacterAnimOption::TOGGLE_AUTO_ANIM:
        Globals::enableCharacterAnim = !Globals::enableCharacterAnim;
        break;
    case CharacterAnimOption::ANIM_1:
        animationCtrl->set(0, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_2:
        animationCtrl->set(1, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_3:
        animationCtrl->set(2, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_4:
        animationCtrl->set(3, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_5:
        animationCtrl->set(4, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_6:
        animationCtrl->set(5, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_7:
        animationCtrl->set(6, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_8:
        animationCtrl->set(7, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_9:
        animationCtrl->set(8, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_10:
        animationCtrl->set(9, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_11:
        animationCtrl->set(10, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_12:
        animationCtrl->set(11, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_13:
        animationCtrl->set(12, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_14:
        animationCtrl->set(13, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_15:
        animationCtrl->set(14, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_16:
        animationCtrl->set(15, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_17:
        animationCtrl->set(16, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_18:
        animationCtrl->set(17, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_19:
        animationCtrl->set(18, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_20:
        animationCtrl->set(19, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_21:
        animationCtrl->set(20, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_22:
        animationCtrl->set(21, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_23:
        animationCtrl->set(22, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_24:
        animationCtrl->set(23, true);
        Globals::enableCharacterAnim = false;
        break;
    default:
        break;
    }

    animationCtrl->play();
    return selectedView;
}
