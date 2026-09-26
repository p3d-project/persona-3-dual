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
    movement->configureMovement(MovementConfig(IWATODAI_DORM_FLOOR_1_MAP_WIDTH,
                                               IWATODAI_DORM_FLOOR_1_MAP_HEIGHT,
                                               &iwatodai_dorm_floor_1_map[0][0],
                                               tileSize,
                                               dbEntry->worldOffsetX,
                                               dbEntry->worldOffsetZ,
                                               characterSize,
                                               speed,
                                               height,
                                               characterTranslate,
                                               characterFacingAngle));
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

void IwatodaiDormView::onEnvironmentUpdate()
{
    // restart after each full cycle completes
    if (!circleHandle.isValid())
    {
        circleHandle =
            animation->sequence()
                .append(
                    animation->animate(triSize).from(10.0f).to(60.0f).duration(500).ease(uiAnimation::Ease::OutBack))
                .append(animation->animate(triSize).from(60.0f).to(10.0f).duration(400).ease(uiAnimation::Ease::InBack))
                .start();
    }

    static constexpr float Z = 0.101f;
    static constexpr float S = 5.47e-4f;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(2));
    glColor3b(0, 200, 220);

    glBegin(GL_TRIANGLES);
    glVertex3f(0, triSize * S, -Z);
    glVertex3f(-triSize * 0.866f * S, -triSize * 0.5f * S, -Z);
    glVertex3f(+triSize * 0.866f * S, -triSize * 0.5f * S, -Z);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FOG);

    glPopMatrix(1);
}

void IwatodaiDormView::cleanup()
{
    circleHandle.cancel();
    animation->cancelAll();
    EnvironmentView::cleanup();
}
