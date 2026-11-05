#pragma once
#include "components/menus/UIMenu.hpp"
#include "controllers/AnimationController.hpp"
#include "data/audioDb.hpp"
#include "managers/UIManager.hpp"
#include "systems/CameraSystem.hpp"
#include <etl/array.h>

enum class DebugOption
{
    SWITCH_VIEW = 0,
    PLAY_CUTSCENE,
    TOGGLE_BILLBOARDS,
    TOGGLE_DEBUG_PRINT,
    PLAY_MUSIC,
    PLAY_CHARACTER_ANIM,
    CYCLE_CAMERA_MODE
};

enum class ViewOption
{
    DISCLAIMER = 0,
    INTRO,
    MAIN_MENU,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    STATION,
    SIGN_CONTRACT,
    PAULOWNIA_MALL
};

enum class CutsceneOption
{
    INTRO_VIDEO = 0,
    CUTSCENE_1,
    CUTSCENE_2
};

enum class SystemOption
{
    TUTORIAL = 0,
    CONFIG,
    DICTIONARY,
    LOAD_DATA,
    SAVE_DATA,
    RETURN_TO_TITLE
};

class PauseMenu : public UIMenu
{
  private:
    PauseMenu() : UIMenu(true) {};
    virtual ~PauseMenu() = default;
    static PauseMenu* instance;

    CameraSystem& cameraSystem = CameraSystem::GetInstance();
    UIManager& ui = UIManager::GetInstance();

    etl::array<CameraMode, 4> cameraModes = {
        CameraMode::Free, CameraMode::Static, CameraMode::CCTV, CameraMode::Follow};

    etl::array<MenuOption, 8> menuOptions = {
        MenuOption{"DEBUG", -1, menuBind(&PauseMenu::openDebugMenu)},
        {"SKILL", -1, menuBind(&PauseMenu::openSkillMenu)},
        {"ITEM", -1, menuBind(&PauseMenu::openItemMenu)},
        {"PERSONA", -1, menuBind(&PauseMenu::openPersonaMenu)},
        {"EQUIP", -1, menuBind(&PauseMenu::openEquipMenu)},
        {"STATUS", -1, menuBind(&PauseMenu::openStatusMenu)},
        {"S.LINK", -1, menuBind(&PauseMenu::openSLinkMenu)},
        {"SYSTEM", -1, menuBind(&PauseMenu::openSystemMenu)},
    };

    etl::array<MenuOption, 7> debugOptions = {
        MenuOption{"Switch View", -1, menuBind(&PauseMenu::openViewMenu)},
        {"Play Cutscene", -1, menuBind(&PauseMenu::openCutsceneMenu)},
        {"Toggle Billboards", -1, menuBind(&PauseMenu::debugOptionSelected)},
        {"Toggle Debug Print", -1, menuBind(&PauseMenu::debugOptionSelected)},
        {"Play Music", -1, menuBind(&PauseMenu::openMusicMenu)},
        {"Play Animations", -1, menuBind(&PauseMenu::openCharacterAnimMenu)},
        {"Cycle Camera Mode", -1, menuBind(&PauseMenu::debugOptionSelected)},
    };

    etl::array<MenuOption, 8> viewOptions = {
        MenuOption{"DisclaimerView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"IntroView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"MainMenuView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"IwatodaiDormView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"IwatodaiStreetView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"StationView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"SignContractView", -1, menuBind(&PauseMenu::viewOptionSelected)},
        {"PaulowniaMallView", -1, menuBind(&PauseMenu::viewOptionSelected)},
    };

    etl::array<MenuOption, 3> cutsceneOptions = {
        MenuOption{"IntroVideo", -1, menuBind(&PauseMenu::cutsceneOptionSelected)},
        {"Cutscene1", -1, menuBind(&PauseMenu::cutsceneOptionSelected)},
        {"Cutscene2", -1, menuBind(&PauseMenu::cutsceneOptionSelected)},
    };

    etl::array<MenuOption, 4> musicOptions = {
        MenuOption{"Battle", -1, menuBind(&PauseMenu::openBattleMusicMenu)},
        {"Location", -1, menuBind(&PauseMenu::openLocationMusicMenu)},
        {"Menu", -1, menuBind(&PauseMenu::openMenuMusicMenu)},
        {"Other", -1, menuBind(&PauseMenu::openOtherMusicMenu)},
    };

    etl::array<MenuOption, 1> battleMusicOptions = {
        MenuOption{"Mass Destruction", -1, menuBind(&PauseMenu::battleMusicOptionSelected)},
    };

    etl::array<MenuOption, 4> locationMusicOptions = {
        MenuOption{"Iwatodai Dorm", -1, menuBind(&PauseMenu::locationMusicOptionSelected)},
        {"Changing Seasons", -1, menuBind(&PauseMenu::locationMusicOptionSelected)},
        {"Color Your Night", -1, menuBind(&PauseMenu::locationMusicOptionSelected)},
        {"Paulownia Mall", -1, menuBind(&PauseMenu::locationMusicOptionSelected)},
    };

    etl::array<MenuOption, 3> menuMusicOptions = {
        MenuOption{"Mistic", -1, menuBind(&PauseMenu::menuMusicOptionSelected)},
        {"Tightrope", -1, menuBind(&PauseMenu::menuMusicOptionSelected)},
        {"Aria of the Soul", -1, menuBind(&PauseMenu::menuMusicOptionSelected)},
    };

    etl::array<MenuOption, 5> otherMusicOptions = {
        MenuOption{"After the Battle", -1, menuBind(&PauseMenu::otherMusicOptionSelected)},
        {"Because I Will Protect You", -1, menuBind(&PauseMenu::otherMusicOptionSelected)},
        {"Club Escapade Nouveau MetalMoth Remix", -1, menuBind(&PauseMenu::otherMusicOptionSelected)},
        {"Peace", -1, menuBind(&PauseMenu::otherMusicOptionSelected)},
        {"Want To Be Close", -1, menuBind(&PauseMenu::otherMusicOptionSelected)},
    };

    etl::array<MenuOption, 9> skillOptions = {
        MenuOption{"Makoto", 0, menuBind(&PauseMenu::skillOptionSelected)},
        {"Yukari", 1, menuBind(&PauseMenu::skillOptionSelected)},
        {"Junpei", 3, menuBind(&PauseMenu::skillOptionSelected)},
        {"Akihiko", 2, menuBind(&PauseMenu::skillOptionSelected)},
        {"Mitsuru", -1, menuBind(&PauseMenu::skillOptionSelected)},
        {"Aigis", -1, menuBind(&PauseMenu::skillOptionSelected)},
        {"Ken", -1, menuBind(&PauseMenu::skillOptionSelected)},
        {"Koromaru", -1, menuBind(&PauseMenu::skillOptionSelected)},
        {"Shinjiro", -1, menuBind(&PauseMenu::skillOptionSelected)},
    };

    etl::array<MenuOption, 3> itemOptions = {
        MenuOption{"Life Stone", -1, menuBind(&PauseMenu::itemOptionSelected)},
        {"Medicine", -1, menuBind(&PauseMenu::itemOptionSelected)},
        {"Bead", -1, menuBind(&PauseMenu::itemOptionSelected)},
    };

    etl::array<MenuOption, 9> equipOptions = {
        MenuOption{"Makoto", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Yukari", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Junpei", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Akihiko", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Mitsuru", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Aigis", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Ken", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Koromaru", -1, menuBind(&PauseMenu::equipOptionSelected)},
        {"Shinjiro", -1, menuBind(&PauseMenu::equipOptionSelected)},
    };

    etl::array<MenuOption, 3> personaOptions = {
        MenuOption{"Jack Frost", -1, menuBind(&PauseMenu::personaOptionSelected)},
        {"Black Frost", -1, menuBind(&PauseMenu::personaOptionSelected)},
        {"King Frost", -1, menuBind(&PauseMenu::personaOptionSelected)},
    };

    etl::array<MenuOption, 9> statsOptions = {
        MenuOption{"Makoto", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Yukari", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Junpei", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Akihiko", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Mitsuru", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Aigis", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Ken", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Koromaru", -1, menuBind(&PauseMenu::statsOptionSelected)},
        {"Shinjiro", -1, menuBind(&PauseMenu::statsOptionSelected)},
    };

    etl::array<MenuOption, 3> sLinkOptions = {
        MenuOption{"Fool", -1, menuBind(&PauseMenu::sLinkOptionSelected)},
        {"Magician", -1, menuBind(&PauseMenu::sLinkOptionSelected)},
        {"Emperor", -1, menuBind(&PauseMenu::sLinkOptionSelected)},
    };

    etl::array<MenuOption, 6> systemOptions = {
        MenuOption{"Tutorial", -1, menuBind(&PauseMenu::systemOptionSelected)},
        {"Config", -1, menuBind(&PauseMenu::systemOptionSelected)},
        {"Dictionary", -1, menuBind(&PauseMenu::systemOptionSelected)},
        {"Load Data", -1, menuBind(&PauseMenu::systemOptionSelected)},
        {"Save Data", -1, menuBind(&PauseMenu::systemOptionSelected)},
        {"Return to Title", -1, menuBind(&PauseMenu::systemOptionSelected)},
    };

    etl::array<MenuOption, 2> skills = {
        MenuOption{"Skill 1", -1, MENU_BIND(PauseMenu, skillActionSelected)},
        {"Skill 2", -1, MENU_BIND(PauseMenu, skillActionSelected)},
    };

    etl::array<MenuOption, 25> characterAnimOptions = {
        MenuOption{"Auto Anim", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0000", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0001", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0002", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0003", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0004", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0005", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0006", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0007", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0008", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0009", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0010", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0011", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0012", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0013", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0014", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0015", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0016", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0017", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0018", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0019", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0020", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0021", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"0022", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
        {"root", -1, menuBind(&PauseMenu::characterAnimOptionSelected)},
    };

    // menu navigation handlers
    void updateCameraOptionName();
    ViewState openDebugMenu();
    ViewState openSkillMenu();
    ViewState openItemMenu();
    ViewState openPersonaMenu();
    ViewState openEquipMenu();
    ViewState openStatusMenu();
    ViewState openSLinkMenu();
    ViewState openSystemMenu();
    ViewState openCharacterAnimMenu();
    ViewState openViewMenu();
    ViewState openCutsceneMenu();
    ViewState openMusicMenu();
    ViewState openBattleMusicMenu();
    ViewState openLocationMusicMenu();
    ViewState openMenuMusicMenu();
    ViewState openOtherMusicMenu();

    // selection handlers
    ViewState debugOptionSelected();
    ViewState viewOptionSelected();
    ViewState cutsceneOptionSelected();
    ViewState skillOptionSelected();
    ViewState skillActionSelected();
    ViewState itemOptionSelected();
    ViewState equipOptionSelected();
    ViewState personaOptionSelected();
    ViewState statsOptionSelected();
    ViewState sLinkOptionSelected();
    ViewState systemOptionSelected();
    ViewState characterAnimOptionSelected();
    ViewState battleMusicOptionSelected();
    ViewState locationMusicOptionSelected();
    ViewState menuMusicOptionSelected();
    ViewState otherMusicOptionSelected();
    ViewState playMusicOptionSelected(AudioType type, int audioId);

    AnimationController* animationCtrl = AnimationController::getInstance();

    void resetHook() override;
    void closeHook() override;

  public:
    static void create();
    static void destroy();
    static PauseMenu* getInstance();

    ViewState updateHook() override;

    bool isClosed = false;
};
