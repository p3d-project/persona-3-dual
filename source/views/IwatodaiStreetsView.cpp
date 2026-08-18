#include "IwatodaiStreetsView.h"
#include "events/BattleEvents.hpp"
#include "events/GenericEvents.hpp"

IwatodaiStreetsView::IwatodaiStreetsView()
{
    // Battle setup
    characterProfiles.push_back(CharacterProfileDb::junpei);
    characterProfiles.push_back(CharacterProfileDb::yukari);

    enemyProfiles.push_back(EnemyProfileDb::cowardlyMaya);
    enemyProfiles.push_back(EnemyProfileDb::mercilessMaya);
}

IwatodaiStreetsView::~IwatodaiStreetsView()
{
    enemyProfiles.clear();
    characterProfiles.clear();
}

void IwatodaiStreetsView::startBattle()
{
    // start battle
    Event::ExecuteBattle msg(CharacterProfileDb::player, characterProfiles, enemyProfiles, battleStartCondition);
    ae::BroadcastEvent(msg);
}

// ----------------------------
// Camera
// ----------------------------
void IwatodaiStreetsView::setCameraConfig()
{
    camConfig.mode = CameraMode::Follow;
    camConfig.initialAngle = 1.5708f * 2;
    camConfig.distance = 1.0f;
    camConfig.height = height + 0.6f;
    camConfig.lookAhead = 0.2f;
    camConfig.angleIncrement = 0.05f;
}

// ----------------------------
// Player controller
// ----------------------------
void IwatodaiStreetsView::setMovementConfig()
{
    movement->configureMovement(MovementConfig(IWATODAI_STREETS_MAP_WIDTH,
                                               IWATODAI_STREETS_MAP_HEIGHT,
                                               &iwatodai_streets_map[0][0],
                                               tileSize,
                                               dbEntry->worldOffsetX,
                                               dbEntry->worldOffsetZ,
                                               characterSize,
                                               speed,
                                               height,
                                               characterTranslate,
                                               characterFacingAngle));
}

void IwatodaiStreetsView::setMusic()
{
    musicCtrl->init((fatBasePath + "music/locations/iwatodaiStreets/changing_seasons.pcm").c_str(), 31.0f, 177.587f);
}

ViewState IwatodaiStreetsView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_0:
        musicCtrl->pause();
        return ViewState::IWATODAI_DORM;

    case TileType::SCENE_1:
        musicCtrl->pause();
        return ViewState::PAULOWNIA_MALL;

    case TileType::SCENE_2:
        musicCtrl->pause();
        return ViewState::STATION;

    case TileType::SHD_W:
    {
        if (!promptDrawn)
        {
            textSub->drawText("Battle Zone", 0, 0, TextColor::White);
            promptDrawn = true;
        }
        if (pressed & KEY_A)
        {
            phase = ViewPhase::Battle;
            prevEnvironmentState = false;
        }

        break;
    }

    default:
        if (promptDrawn)
        {
            textSub->clearScreen();
            promptDrawn = false;
        }
        break;
    }

    return ViewState::KEEP_CURRENT;
}

void IwatodaiStreetsView::setTextConfig()
{
    text->configureText(TextConfig(textVideoBuffer, &FONT_NAME, FONT_SIZE));
    textSub->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));
}

void IwatodaiStreetsView::setupUI()
{
    textMenu->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));

    battleMenuCmpt = BattleMenuComponent::getInstance();
    pauseMenuCmpt = PauseMenuComponent::getInstance();

    menuHUDScreen = MenuHUDScreen::getInstance();

    std::array<UIScreen*, 7> screens = {menuHUDScreen};
    std::array<UIMenu*, 10> menus = {pauseMenuCmpt, battleMenuCmpt};

    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, screens});
    ae::BroadcastEvent(Event::ConfigureUIMenu{textMenu, menus});
}
