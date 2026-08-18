#include "PauseMenuComponent.h"
#include "core/globals.h"
#include <nds.h>
#include <string>

// sfx
#include "soundbank.h"
// dialogue
#include "dialogue/demo_dialogue.h"

PauseMenuComponent* PauseMenuComponent::instance = nullptr;

void PauseMenuComponent::create()
{
    if (instance == nullptr)
    {
        instance = new PauseMenuComponent();
    }
}

void PauseMenuComponent::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

PauseMenuComponent* PauseMenuComponent::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

void PauseMenuComponent::resetHook()
{
    pauseMessage = "Pause";
    options = menuOptions;
    optionCount = MENU_OPTIONS;
    isClosed = false;

    if (demo_dialogue_bg_slot != -1)
    {
        rm.hideBg(demo_dialogue_bg_slot);
    }

    if (pauseMenu == nullptr)
    {
        pauseMenu = engine.CreateEntity();
        dialogue = engine.CreateComponent<DialogueComponent>();
        pauseMenu->AddComponent(dialogue);
    }
}

ViewState PauseMenuComponent::updateHook()
{
    // dialogue should be started, but has not
    if (isDialogueStarted && !dialogue->IsActive())
    {
        text->clearScreen();
        dialogue->start();
        isDialogueStarted = false;
    }

    // dialogue controller takes full control when active
    if (dialogue->IsActive())
    {
        return ViewState::KEEP_CURRENT;
    }

    if (demo_dialogue_bg_slot != -1)
    {
        rm.hideBg(demo_dialogue_bg_slot);
    }

    return ViewState::DEFAULT;
}

void PauseMenuComponent::closeHook()
{
    resetMenu();

    // set flag
    isClosed = true;
}

// menu navigation handlers

ViewState PauseMenuComponent::openDebugMenu()
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
    return changeMenu(debugOptions, DEBUG_OPTIONS);
}

ViewState PauseMenuComponent::openSkillMenu()
{
    return changeMenu(skillOptions, SKILL_OPTIONS);
}

ViewState PauseMenuComponent::openItemMenu()
{
    return changeMenu(itemOptions, ITEM_OPTIONS);
}

ViewState PauseMenuComponent::openPersonaMenu()
{
    return changeMenu(personaOptions, PERSONA_OPTIONS);
}

ViewState PauseMenuComponent::openEquipMenu()
{
    return changeMenu(equipOptions, EQUIP_OPTIONS);
}

ViewState PauseMenuComponent::openStatusMenu()
{
    return changeMenu(statsOptions, STATS_OPTIONS);
}

ViewState PauseMenuComponent::openSLinkMenu()
{
    return changeMenu(sLinkOptions, S_LINK_OPTIONS);
}

ViewState PauseMenuComponent::openSystemMenu()
{
    return changeMenu(systemOptions, SYSTEM_OPTIONS);
}

ViewState PauseMenuComponent::openCharacterAnimMenu()
{
    return changeMenu(characterAnimOptions, CHARACTER_ANIM_OPTIONS);
}

// selection handlers

ViewState PauseMenuComponent::skillOptionSelected()
{
    return changeMenu(skills, SKILLS);
}

ViewState PauseMenuComponent::itemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::equipOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::personaOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::statsOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::sLinkOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::systemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::debugOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<DebugOption>(selectedOption))
    {
    case DebugOption::DISCLAIMER_VIEW:
        selectedView = ViewState::DISCLAIMER;
        break;
    case DebugOption::INTRO_VIEW:
        selectedView = ViewState::INTRO;
        break;
    case DebugOption::MAIN_MENU_VIEW:
        selectedView = ViewState::MAIN_MENU;
        break;
    case DebugOption::IWATODAI_DORM_VIEW:
        selectedView = ViewState::IWATODAI_DORM;
        break;
    case DebugOption::IWATODAI_STREETS_VIEW:
        selectedView = ViewState::IWATODAI_STREETS;
        break;
    case DebugOption::STATION_VIEW:
        selectedView = ViewState::STATION;
        break;
    case DebugOption::SIGN_CONTRACT_VIEW:
        selectedView = ViewState::SIGN_CONTRACT;
        break;
    case DebugOption::PAULOWNIA_MALL_VIEW:
        selectedView = ViewState::PAULOWNIA_MALL;
        break;
    case DebugOption::INTRO_VIDEO:
        selectedView = ViewState::INTRO_VIDEO;
        break;
    case DebugOption::CUTSCENE_1:
        selectedView = ViewState::CUTSCENE_1;
        break;
    case DebugOption::CUTSCENE_2:
        selectedView = ViewState::CUTSCENE_2;
        break;
    case DebugOption::DEBUG_DIALOGUE:
        demo_yukari_kenji_argument_load();
        dialogue->configureDialogue(
            DialogueConfig(demo_yukari_kenji_argument_first(), demo_yukari_kenji_argument_load_bg, text));
        isDialogueStarted = true;
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case DebugOption::TOGGLE_BILLBOARDS:
        Globals::enableBillboards = !Globals::enableBillboards;
        isActive = false;
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case DebugOption::TOGGLE_DEBUG_PRINT:
        Globals::enableDebugPrint = !Globals::enableDebugPrint;
        isActive = false;
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
        isActive = false;
        openDebugMenu();
        selectedView = ViewState::KEEP_CURRENT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }
    return selectedView;
}

ViewState PauseMenuComponent::characterAnimOptionSelected()
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

    isActive = false;
    animationCtrl->play();
    return selectedView;
}
