#include "components/menus/UIMenu.hpp"

enum class MainMenuOptions
{
    LOAD_GAME = 0,
    SETTINGS,
    RETURN_TO_TITLE,
};

enum class LevelOptions
{
    START_GAME = 0,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    STATION,
    PAULOWNIA_MALL,
    SIGN_CONTRACT,
};

//No settings as of now
enum class SettingOptions
{
};

class MainMenu : public UIMenu
{
  private:
    MainMenu() {};
    virtual ~MainMenu() = default;
    static MainMenu* instance;

    MenuOption mainMenuOptions[3] = {
        {"Load Game", -1, menuBind(&MainMenu::mainMenuOptionSelected)},
        {"Settings", -1, menuBind(&MainMenu::mainMenuOptionSelected)},
        {"Return to Title", -1, menuBind(&MainMenu::mainMenuOptionSelected)},
    };

    MenuOption levelOptions[6] = {
        {"Start Game", -1, menuBind(&MainMenu::levelOptionSelected)},
        {"Iwatodai Dorm", -1, menuBind(&MainMenu::levelOptionSelected)},
        {"Iwatodai Streets", -1, menuBind(&MainMenu::levelOptionSelected)},
        {"Station", -1, menuBind(&MainMenu::levelOptionSelected)},
        {"Paulownia Mall", -1, menuBind(&MainMenu::levelOptionSelected)},
        {"Sign Contract", -1, menuBind(&MainMenu::levelOptionSelected)},
    };

    MenuOption settingOptions[1] = {
        {versionLabel, -1, nullptr},
    };

    // option handlers
    ViewState mainMenuOptionSelected();
    ViewState levelOptionSelected();
    ViewState settingOptionSelected();

    void resetHook() override;
    void closeHook() override;

    // helper
    void updateSave();

  public:
    char versionLabel[16]{};
    static void create();
    static void destroy();
    static MainMenu* getInstance();
};
