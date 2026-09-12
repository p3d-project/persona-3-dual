#include <memory>
#include <string>

#include <fat.h>
#include <maxmod9.h>
#include <nds.h>

// states
#include "views/BaseView.hpp"
#include "views/DisclaimerView.hpp"
#include "views/IntroView.hpp"
#include "views/IwatodaiDormView.hpp"
#include "views/IwatodaiStreetsView.hpp"
#include "views/MainMenuView.hpp"
#include "views/PaulowniaMallView.hpp"
#include "views/SignContractView.hpp"
#include "views/StationView.hpp"
#include "views/VideoView.hpp"

// sfx
#include "soundbank_bin.h"

// DBs
#include "battleActions/armours/ArmourDb.hpp"
#include "battleActions/enemies/EnemyProfileDb.hpp"
#include "battleActions/party/CharacterProfileDb.hpp"
#include "battleActions/personas/PersonaDb.hpp"
#include "battleActions/shoes/ShoeDb.hpp"
#include "battleActions/skills/SkillDb.hpp"
#include "battleActions/weapons/WeaponDb.hpp"

// game engine
GameEngine engine;
ae::Entity* player;

// variables
volatile int frame = 0;
volatile u32 systemKeysDown = 0;
volatile u32 systemKeysHeld = 0;
std::string fatBasePath = "";
Save saveData;
ViewState nextView = ViewState::DEFAULT;

std::unique_ptr<BaseView> currentView;

void SwitchView(BaseView* newView)
{
    // cleanup any existing view
    if (currentView != nullptr)
    {
        currentView->cleanup();
    }

    // load new view
    currentView.reset(newView);
    if (currentView != nullptr)
    {
        currentView->init();
    }
}

// fn for the interrupt
void Vblank()
{
    frame = frame + 1;
}

// TODO: add doxyen docs
void NDSPollInputCallback()
{
    scanKeys();
    systemKeysDown = keysDown();
    systemKeysHeld = keysHeld();
}

// TODO: add doxyen docs
void NDSComputeCallback()
{
    //...
}

int main(int argc, char* argv[])
{
    irqSet(IRQ_VBLANK, Vblank);

    // initialize DLDI/FAT
    if (!fatInitDefault())
    {
        consoleDemoInit();
        printf("FAT initialization failed!\nPlease ensure the SD card is inserted.\n");
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    // dynamically resolve runtime path using argv[0]
    if (argc > 0 && argv[0] != nullptr)
    {
        std::string execPath(argv[0]);
        size_t lastSlash = execPath.find_last_of('/');

        if (lastSlash != std::string::npos)
        {
            fatBasePath = execPath.substr(0, lastSlash + 1) + "data/";
        }
    }

    // initialize maxmod (for audio)
    mm_ds_system sys;
    sys.mod_count = 0;
    sys.samp_count = 0;
    sys.mem_bank = 0;
    mmInit(&sys);

    // initialize maxmod (for sfx)
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // setup db's. DO NOT CHANGE order
    WeaponDb::Initialize();
    SkillDb::Initialize();
    ArmourDb::Initialize();
    ShoeDb::Initialize();
    PersonaDb::Initialize();
    EnemyProfileDb::Initialize();
    CharacterProfileDb::Initialize();

    // setup globals
    Globals::enableDebugPrint = false;
    Globals::enableBillboards = true;
    Globals::enableCharacterAnim = true;
    Globals::isPauseMenuActive = false;

    // seed random using DS hardware timer
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
    srand(TIMER0_DATA);

    // set platform hooks
    engine.SetComputeCallback(&NDSComputeCallback);
    engine.SetComputeEnabled(true);
    engine.SetPollInputCallback(&NDSPollInputCallback);
    engine.SetPollingEnabled(true);

    // register singletons
    engine.RegisterSystem(&BattleSystem::GetInstance());
    engine.RegisterSystem(&CameraSystem::GetInstance());
    engine.RegisterSystem(&SaveSystem::GetInstance());
    engine.RegisterSystem(&TextSystem::GetInstance());
    engine.RegisterSystem(&UISystem::GetInstance());

    engine.RegisterManager(&MathManager::GetInstance());
    engine.RegisterManager(&IOManager::GetInstance());
    engine.RegisterManager(&TextManager::GetInstance());
    engine.RegisterManager(&UIManager::GetInstance());

    // initialize engine
    engine.InitAll();

    // set up initial game state
    // create entity
    player = engine.CreateEntity();

    // load save data
    ae::BroadcastEvent(Event::ReadSave{});

    // Default is DisclaimerView
    SwitchView(new DisclaimerView());

    // TODO: set to constant tied to VBlank
    const ae::q20_12_t dt = MathManager::GetInstance().div(ae::q20_12_t{1}, ae::q20_12_t{60});

    while (1)
    {
        swiWaitForVBlank();

        // Poll Input -> Update Systems -> Update Components -> Process Managers -> Compute
        engine.Tick(dt);

        // check state of currentView
        if (currentView != nullptr)
        {
            ViewState nextState;
            if (nextView != ViewState::DEFAULT)
            {
                nextState = nextView;
                nextView = ViewState::DEFAULT;
            }
            else
            {
                nextState = currentView->update();
            }

            switch (nextState)
            {
            case ViewState::INTRO:
            {
                SwitchView(new IntroView());
                break;
            }

            case ViewState::MAIN_MENU:
            {
                SwitchView(new MainMenuView());
                break;
            }

            case ViewState::IWATODAI_DORM:
            {
                SwitchView(new IwatodaiDormView());
                break;
            }

            case ViewState::IWATODAI_STREETS:
            {
                SwitchView(new IwatodaiStreetsView());
                break;
            }

            case ViewState::DISCLAIMER:
            {
                SwitchView(new DisclaimerView());
                break;
            }

            case ViewState::INTRO_VIDEO:
            {
                SwitchView(new VideoView(saveData.introVideoPath, ViewState::INTRO));
                break;
            }

            case ViewState::CUTSCENE_1:
            {
                SwitchView(new VideoView("cutscene-1.vid", ViewState::SIGN_CONTRACT));
                break;
            }

            case ViewState::SIGN_CONTRACT:
            {
                SwitchView(new SignContractView());
                break;
            }

            case ViewState::CUTSCENE_2:
            {
                SwitchView(new VideoView("cutscene-2.vid", ViewState::IWATODAI_DORM));
                break;
            }

            case ViewState::STATION:
            {
                SwitchView(new StationView());
                break;
            }

            case ViewState::PAULOWNIA_MALL:
            {
                SwitchView(new PaulowniaMallView());
                break;
            }

            default:
            {
                break;
            }
            }
        }

        bgUpdate();
        oamUpdate(&oamMain);
    }

    engine.ShutdownAll();

    return 0;
}
