#pragma once
#include "components/DialogueComponent.hpp"
#include "components/menu/UIMenu.h"
#include "controllers/AnimationController.h"
#include "core/globals.h"
#include "dialogue/demo_dialogue.h"
#include "managers/RenderManager.hpp"
#include "systems/CameraSystem.hpp"
#include <array>

#define MENU_OPTIONS 8
#define SKILL_OPTIONS 9
#define ITEM_OPTIONS 3
#define EQUIP_OPTIONS 9
#define PERSONA_OPTIONS 3
#define STATS_OPTIONS 9
#define S_LINK_OPTIONS 3
#define SYSTEM_OPTIONS 6
#define DEBUG_OPTIONS 16
#define CHARACTER_ANIM_OPTIONS 25
#define SKILLS 2

class PauseMenuComponent : public UIMenu
{
  private:
    PauseMenuComponent() {};
    virtual ~PauseMenuComponent() = default;
    static PauseMenuComponent* instance;

    CameraSystem& cameraSystem = CameraSystem::GetInstance();
    RenderManager& rm = RenderManager::GetInstance();

    std::array<CameraMode, 4> cameraModes = {
        CameraMode::Free, CameraMode::Static, CameraMode::CCTV, CameraMode::Follow};

    MenuOption menuOptions[MENU_OPTIONS] = {
        {"Debug", -1, MENU_BIND(PauseMenuComponent, openDebugMenu)},
        {"Skill", -1, MENU_BIND(PauseMenuComponent, openSkillMenu)},
        {"Item", -1, MENU_BIND(PauseMenuComponent, openItemMenu)},
        {"Persona", -1, MENU_BIND(PauseMenuComponent, openPersonaMenu)},
        {"Equip", -1, MENU_BIND(PauseMenuComponent, openEquipMenu)},
        {"Status", -1, MENU_BIND(PauseMenuComponent, openStatusMenu)},
        {"S.Link", -1, MENU_BIND(PauseMenuComponent, openSLinkMenu)},
        {"System", -1, MENU_BIND(PauseMenuComponent, openSystemMenu)},
    };

    MenuOption debugOptions[DEBUG_OPTIONS] = {
        {"DisclaimerView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IntroView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"MainMenuView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IwatodaiDormView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IwatodaiStreetView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"StationView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"SignContractView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"PaulowniaMallView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IntroVideo", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Cutscene1", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Cutscene2", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Debug Dialogue", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Toggle Billboards", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Toggle Debug Print", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Play Character Animations", -1, MENU_BIND(PauseMenuComponent, openCharacterAnimMenu)},
        {"Cycle Camera Mode", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
    };

    MenuOption skillOptions[SKILL_OPTIONS] = {
        {"Makoto", 0, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Yukari", 1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Junpei", 3, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Akihiko", 2, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Mitsuru", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Aigis", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Ken", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Koromaru", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Shinjiro", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
    };

    MenuOption itemOptions[ITEM_OPTIONS] = {
        {"Life Stone", -1, MENU_BIND(PauseMenuComponent, itemOptionSelected)},
        {"Medicine", -1, MENU_BIND(PauseMenuComponent, itemOptionSelected)},
        {"Bead", -1, MENU_BIND(PauseMenuComponent, itemOptionSelected)},
    };

    MenuOption equipOptions[EQUIP_OPTIONS] = {
        {"Makoto", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Yukari", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Junpei", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Akihiko", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Mitsuru", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Aigis", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Ken", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Koromaru", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Shinjiro", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
    };

    MenuOption personaOptions[PERSONA_OPTIONS] = {
        {"Jack Frost", -1, MENU_BIND(PauseMenuComponent, personaOptionSelected)},
        {"Black Frost", -1, MENU_BIND(PauseMenuComponent, personaOptionSelected)},
        {"King Frost", -1, MENU_BIND(PauseMenuComponent, personaOptionSelected)},
    };

    MenuOption statsOptions[STATS_OPTIONS] = {
        {"Makoto", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Yukari", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Junpei", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Akihiko", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Mitsuru", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Aigis", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Ken", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Koromaru", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Shinjiro", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
    };

    MenuOption sLinkOptions[S_LINK_OPTIONS] = {
        {"Fool", -1, MENU_BIND(PauseMenuComponent, sLinkOptionSelected)},
        {"Magician", -1, MENU_BIND(PauseMenuComponent, sLinkOptionSelected)},
        {"Emperor", -1, MENU_BIND(PauseMenuComponent, sLinkOptionSelected)},
    };

    MenuOption systemOptions[SYSTEM_OPTIONS] = {
        {"Tutorial", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Config", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Dictionary", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Load Data", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Save Data", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Return to Title", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
    };

    MenuOption skills[SKILLS] = {
        {"Skill 1", -1, nullptr},
        {"Skill 2", -1, nullptr},
    };

    MenuOption characterAnimOptions[CHARACTER_ANIM_OPTIONS] = {
        {"Toggle Auto Animations", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0000", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0001", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0002", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0003", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0004", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0005", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0006", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0007", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0008", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0009", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0010", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0011", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0012", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0013", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0014", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0015", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0016", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0017", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0018", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0019", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0020", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0021", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0022", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"root", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
    };

    // menu navigation handlers
    ViewState openDebugMenu();
    ViewState openSkillMenu();
    ViewState openItemMenu();
    ViewState openPersonaMenu();
    ViewState openEquipMenu();
    ViewState openStatusMenu();
    ViewState openSLinkMenu();
    ViewState openSystemMenu();
    ViewState openCharacterAnimMenu();

    // selection handlers
    ViewState debugOptionSelected();
    ViewState skillOptionSelected();
    ViewState itemOptionSelected();
    ViewState equipOptionSelected();
    ViewState personaOptionSelected();
    ViewState statsOptionSelected();
    ViewState sLinkOptionSelected();
    ViewState systemOptionSelected();
    ViewState characterAnimOptionSelected();

    bool isDialogueStarted = false;
    ae::Entity* pauseMenu = nullptr;
    DialogueComponent* dialogue = nullptr;
    AnimationController* animationCtrl = AnimationController::getInstance();

    void resetHook() override;
    void closeHook() override;

  public:
    static void create();
    static void destroy();
    static PauseMenuComponent* getInstance();

    ViewState updateHook() override;

    bool isClosed = false;
};
