#include "IwatodaiDormView.hpp"
#include "core/globals.hpp"
#include "events/UIEvents.hpp"
#include "types/CameraTypes.hpp"

#include "data/environmentDb.hpp"
#include "demo/demo_dialogue.hpp"
#include "maps/iwatodai_dorm_floor_1.hpp"

IwatodaiDormView::IwatodaiDormView()
{
}

void IwatodaiDormView::setupCamera()
{
    camConfig.mode = CameraMode::Follow;
    camConfig.initialAngle = ae::q20_12_t{-1.6};
    camConfig.distance = ae::q20_12_t{0.8};
    camConfig.height = height + ae::q20_12_t{0.6};
    camConfig.lookAhead = ae::q20_12_t{0.2};
    camConfig.angleIncrement = ae::q20_12_t{0.07};
    camConfig.isRotationLocked = true;
}

void IwatodaiDormView::setupMusic()
{
    musicCmpt->registerMusic(
        (fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.qoa").c_str(), ae::q20_12_t{1.3}, ae::q20_12_t{-1});
}
void IwatodaiDormView::setupMovement()
{
    //first flight, including its initial bend
    static const StairSegment stairs[] = {{ae::q20_12_t{0},
                                           ae::q20_12_t{-2.872},
                                           ae::q20_12_t{-0.897},
                                           ae::q20_12_t{-3.051},
                                           ae::q20_12_t{0.24},
                                           ae::q20_12_t{0},
                                           ae::q20_12_t{0.157}},

                                          {ae::q20_12_t{-0.897},
                                           ae::q20_12_t{-3.051},
                                           ae::q20_12_t{-1.526},
                                           ae::q20_12_t{-3.051},
                                           ae::q20_12_t{0.27},
                                           ae::q20_12_t{0.157},
                                           ae::q20_12_t{0.503}},

                                          //second flight, after the landing
                                          {ae::q20_12_t{-1.840},
                                           ae::q20_12_t{-2.782},
                                           ae::q20_12_t{-1.840},
                                           ae::q20_12_t{-1.885},
                                           ae::q20_12_t{0.28},
                                           ae::q20_12_t{0.503},
                                           ae::q20_12_t{1.041}}};
    //landing between the two flights
    static const HeightRegion landings[] = {

        {ae::q20_12_t{-2.154}, ae::q20_12_t{-1.526}, ae::q20_12_t{-3.410}, ae::q20_12_t{-2.782}, ae::q20_12_t{0.503}}};
    MovementConfig cfg(IWATODAI_DORM_FLOOR_1_MAP_WIDTH,
                       IWATODAI_DORM_FLOOR_1_MAP_HEIGHT,
                       &iwatodai_dorm_floor_1_map[0][0],
                       tileSize,
                       dbEntry->worldOffsetX,
                       dbEntry->worldOffsetZ,
                       characterSize,
                       speed,
                       height,
                       characterTranslate,
                       characterFacingAngle);

    cfg.stairs = stairs;
    cfg.stairCount = 3;

    cfg.landings = landings;
    cfg.landingCount = 1;

    movement->configureMovement(cfg);
}

ViewState IwatodaiDormView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_1:
    {
        return ViewState::PAULOWNIA_MALL;
    }

    case TileType::SCENE_0:
    {
        return ViewState::IWATODAI_STREETS;
    }

    case TileType::C_AK:
    {
        // start dialogue
        if (!promptDrawn)
        {
            menuHUDScreen->setTextContent("\xFF\x02\x01Talk", TextColor::DualGreen);
            promptDrawn = true;
        }
        if (pressed & KEY_A)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::DIALOGUE;
        }
        break;
    }

    default:
    {
        if (promptDrawn)
        {
            menuHUDScreen->setTextContent("Dorm", TextColor::White);
            promptDrawn = false;
        }
        break;
    }
    }
    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::setupDialogue()
{
    dialogueFirstLine = demo_dialogue_init();
    dialogue->configureDialogue(DialogueConfig(&demo_dialogue_spritePayloads, textSub, textSubAlt, dialogueScreen));
}

void IwatodaiDormView::setupText()
{
    text->configureText(TextConfig(textVideoBuffer, &fontName, fontSize));
    textSub->configureText(TextConfig(textVideoBufferSub, &fontName, fontSize));
    textSubAlt->configureText(TextConfig(textVideoBufferSub, &fontNameAlt, fontSizeAlt));
}

void IwatodaiDormView::setupUI()
{
    // setup pause menu
    pauseMenuCmpt = PauseMenu::getInstance();

    menuHUDScreen = MenuHUDScreen::getInstance();
    dialogueScreen = DialogueScreen::getInstance();

    menuHUDScreen->setTextComponent(textSub);
    menuHUDScreen->setTextContent("Dorm", TextColor::White);

    std::array<UIScreen*, 5> screens = {menuHUDScreen, dialogueScreen};
    std::array<UIMenu*, 10> menus = {pauseMenuCmpt};

    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, screens});
    ae::BroadcastEvent(Event::ConfigureUIMenu{textSub, menus});
}
