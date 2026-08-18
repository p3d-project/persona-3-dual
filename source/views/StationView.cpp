#include "StationView.h"

StationView::StationView()
{
}

void StationView::setMusic()
{
    musicCtrl->init(
        (fatBasePath + "music/locations/paulowniaMall/station/paulownia_mall.pcm").c_str(), 2.002f, 73.939f);
}

void StationView::setMovementConfig()
{
    movement->configureMovement(MovementConfig(STATION_MAP_WIDTH,
                                               STATION_MAP_HEIGHT,
                                               &station_map[0][0],
                                               tileSize,
                                               dbEntry->worldOffsetX,
                                               dbEntry->worldOffsetZ,
                                               characterSize,
                                               speed,
                                               height,
                                               characterTranslate,
                                               characterFacingAngle));
}

ViewState StationView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_0:
        return ViewState::PAULOWNIA_MALL;
    default:
        break;
    }

    return ViewState::KEEP_CURRENT;
}

void StationView::setTextConfig()
{
    text->configureText(TextConfig(textVideoBuffer, &FONT_NAME, FONT_SIZE));
    textSub->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));
}

void StationView::setupUI()
{
    textMenu->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));

    pauseMenuCmpt = PauseMenuComponent::getInstance();

    menuHUDScreen = MenuHUDScreen::getInstance();

    std::array<UIScreen*, 7> screens = {menuHUDScreen};
    std::array<UIMenu*, 10> menus = {pauseMenuCmpt};

    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, screens});
    ae::BroadcastEvent(Event::ConfigureUIMenu{textMenu, menus});
}
