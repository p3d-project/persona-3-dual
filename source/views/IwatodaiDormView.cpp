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
    using q = ae::q20_12_t;
    static const StairSegment stairs[] = {//first flight, including its initial bend
                                          {q{0}, q{-2.872}, q{-0.897}, q{-3.051}, q{0.24}, q{0}, q{0.157}},

                                          {q{-0.897}, q{-3.051}, q{-1.526}, q{-3.051}, q{0.27}, q{0.157}, q{0.503}},

                                          //second flight, after the landing
                                          {q{-1.840}, q{-2.782}, q{-1.840}, q{-1.885}, q{0.28}, q{0.503}, q{1.041}}};

    static const HeightRegion landings[] = {//landing between the two flights
                                            {q{-2.154}, q{-1.526}, q{-3.410}, q{-2.782}, q{0.503}}};
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
            textSub->drawText("\xFF\x02\x01Talk", 0, 0, TextColor::Black);
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
            textSub->clearScreen();
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

    std::array<UIScreen*, 5> screens = {menuHUDScreen, dialogueScreen};
    std::array<UIMenu*, 10> menus = {pauseMenuCmpt};

    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, screens});
    ae::BroadcastEvent(Event::ConfigureUIMenu{textSub, menus});
}
