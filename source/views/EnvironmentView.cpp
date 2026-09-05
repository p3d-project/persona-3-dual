#include "EnvironmentView.hpp"
#include "core/globals.hpp"
#include "models/makoto.hpp"
#include "systems/BattleSystem.hpp"

#include <nds.h>
#include <string>

namespace
{
/**
 * @brief Strips the compiled ".img.bin" suffix from a texture filename to
 *        recover the base name expected by loadGrit.
 *
 * environmentDb.cpp stores the *compiled* texture filename, e.g.
 * "f007_002wall01.img.bin", but loadGrit wants the .grit base name instead
 * (e.g. "f007_002wall01").
 *
 * @param compiledFileName The compiled texture filename as stored in the
 *                          environment database (e.g. "name.img.bin").
 * @return The same name with a trailing ".img.bin" suffix removed, or the
 *         name unchanged if it does not end with that suffix.
 */
std::string gritBaseName(const char* compiledFileName)
{
    std::string name(compiledFileName);
    static const std::string suffix = ".img.bin";
    if (name.size() > suffix.size() && name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
        name.erase(name.size() - suffix.size());
    }
    return name;
}
} // namespace

// -------------------------------------------------
// Models

const unsigned int* EnvironmentView::loadBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphics->loadGraphic(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

void EnvironmentView::setupModel()
{
    GraphicAsset modelTextures[MODEL_MAKOTO_TEX_COUNT] = {};
    const unsigned int* bitmapsMakoto[MODEL_MAKOTO_TEX_COUNT] = {nullptr};

    const std::string basePath = fatBasePath + "models/makoto/";

    // load textures
    bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_0] =
        loadBitmap(basePath + "makoto_texture_0", modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_0]);
    bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_1] =
        loadBitmap(basePath + "makoto_texture_1", modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_1]);
    bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_2] =
        loadBitmap(basePath + "makoto_texture_2", modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_2]);
    bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_3] =
        loadBitmap(basePath + "makoto_texture_3", modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_3]);
    bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_4] =
        loadBitmap(basePath + "makoto_texture_4", modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_4]);

    makoto_loadTextures(*animationCtrl, (const unsigned int**)bitmapsMakoto);

    // unload texture graphics
    graphics->unloadGraphic(modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_0]);
    graphics->unloadGraphic(modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_1]);
    graphics->unloadGraphic(modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_2]);
    graphics->unloadGraphic(modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_3]);
    graphics->unloadGraphic(modelTextures[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_4]);
}

// -------------------------------------------------
// Environment

void EnvironmentView::setupEnvironment()
{
    GraphicAsset envTextures[MAX_ENVIRONMENT_TEXTURES] = {};
    std::array<const unsigned int*, MAX_ENVIRONMENT_TEXTURES> bitmapsEnv = {nullptr};

    const std::string basePath = fatBasePath + "environments/" + dbEntry->name + "/";

    for (int i = 0; i < dbEntry->textureCount; ++i)
    {
        bitmapsEnv[i] = loadBitmap(basePath + gritBaseName(dbEntry->textures[i].name), envTextures[i]);
    }

    if (!env.load(dbEntry, bitmapsEnv))
    {
        textSub->drawText(
            "EnvironmentView: failed to load environment " + std::string(dbEntry->name), 0, 0, TextColor::Red);
    }

    for (int i = 0; i < dbEntry->textureCount; ++i)
    {
        graphics->unloadGraphic(envTextures[i]);
    }
}

// -------------------------------------------------
// Lifecycle

void EnvironmentView::init()
{
    // clearing so nothing from the previous enviorment shows during load
    glClearColor(0, 0, 0, 31);
    glClearDepth(0x7FFF);
    glFlush(0);
    swiWaitForVBlank();

    if (environment == nullptr)
    {
        environment = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        text = engine.CreateComponent<TextComponent>();
        textSub = engine.CreateComponent<TextComponent>();
        textSubAlt = engine.CreateComponent<TextComponent>();

        environment->AddComponent(graphics);
        environment->AddComponent(text);
        environment->AddComponent(textSub);
        environment->AddComponent(textSubAlt);
    }

    if (player != nullptr)
    {
        movement = engine.CreateComponent<MovementComponent>();
        dialogue = engine.CreateComponent<DialogueComponent>();

        player->AddComponent(movement);
        player->AddComponent(dialogue);
    }

    // set modes
    videoSetMode(MODE_5_3D | DISPLAY_BG3_ACTIVE);
    videoSetModeSub(MODE_3_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE);

    // set vram
    vramSetBankA(VRAM_A_TEXTURE_SLOT0); // texture slot 0
    vramSetBankB(VRAM_B_TEXTURE_SLOT1); // texture slot 1

    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    vramSetBankE(VRAM_E_MAIN_BG);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    bgExtPaletteEnableSub();

    // 3D init
    glInit();
    glEnable(GL_ANTIALIAS);  // cleans up edges
    glEnable(GL_TEXTURE_2D); // for textures
    // glEnable(GL_BLEND);      // useful for UI
    glEnable(GL_FOG);     // fog effect
    glEnable(GL_OUTLINE); // stylistic outline

    glClearColor(0, 0, 0, 31);
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    // viewport
    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // zNear is how close the camera can see, zFar is the maximum draw distance
    gluPerspective(55, 256.0 / 192.0, 0.1, 40);

    // outline
    glSetOutlineColor(0, RGB15(0, 0, 0));

    // fog
    // setup color
    glFogColor(22, 25, 28, 31); // daytime blue
    // glFogColor(30, 25, 16, 31);  // evening orange
    // glFogColor(16, 17, 19, 31);  // rainy gray

    // how much depth difference there is between table entries
    glFogShift(shift);
    // depth at which the fog starts (and the table starts applying)
    glFogOffset(depth);

    // generate a linear density table
    uint8_t density = 0;
    for (uint8_t i = 0; i < 32; ++i) // it has 32 steps
    {
        glFogDensity(i, density);
        // exponentially increase mass the furthur back the fog is
        density += (mass * i) >> 2;

        // entries are 7 bit, so cap the density to 127
        if (density > 127)
        {
            density = 127;
        }
    }

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG);
    glColor3b(255, 255, 255);

    dbEntry = getEnvironmentDbEntry();
    if (!dbEntry)
    {
        sassert(false, "EnvironmentView::init - no EnvironmentDbEntry for this room");
        return;
    }

    // setup sub screen
    // https://mtheall.com/vram.html#SUB=1&T0=1&NT0=512&MB0=2&TB0=1&S0=0&T1=3&NT1=128&MB1=5&TB1=0&T2=1&NT2=512&MB2=3&TB2=3&S2=0&T3=1&NT3=512&MB3=4&TB3=5&S3=0
    bgSharedSub1 = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSharedSub2 = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 2, 2);
    bgSharedSub3 = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 4, 3);

    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub1), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub2), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub3), 2048);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(bgSharedSub1, 1);
    bgSetPriority(bgSharedSub2, 2);
    bgSetPriority(bgSharedSub3, 3);
    bgUpdate();

    // setup MovementComponent on player entity (room-specific map/tuning, generic call site)
    setupMovement();
    movement->start();

    setupCamera();
    ae::BroadcastEvent(Event::ConfigureCamera(camConfig));

    // setup character model (identical across rooms)
    std::string modelPath = fatBasePath + "models/";
    animationCtrl->loadModel((modelPath + "makoto/makoto.bin").c_str());
    setupModel();

    // setup main screen text engine
    int bgText = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    textVideoBuffer = (uint16_t*)bgGetGfxPtr(bgText);
    bgSetPriority(bgText, 0); // set text layer on main to be on top of 3D view

    // setup sub screen text engine
    int bgTextSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    textVideoBufferSub = (uint16_t*)bgGetGfxPtr(bgTextSub);
    bgSetPriority(bgTextSub, 0);

    // config text/textSub
    setupText();

    // setup environment geometry/textures (fully generic, data-driven)
    setupEnvironment();

    // setup UI
    // NOTE: bg 0 is the 3D view
    bgMain = {1, 2};
    // NOTE: Setting the first index to anything other than bgSharedSub results in black bg (but sprites still load)
    bgSub = {bgSharedSub1, bgSharedSub2, bgSharedSub3};

    // initialize sub sprite engine with 1D mapping, 128 byte boundry, external palette support
    oamInit(&oamSub, SpriteMapping_1D_128, true);

    // setup UIScreen, UIMenu
    setupUI();

    // setup dialogue
    setupDialogue();

    // setup music (room-specific path/loop points)
    setupMusic();

    // setup view phases
    prevPauseState = false;
    prevDialogueState = false;
    prevEnvironmentState = false;
    isBattleMenuActive = false;
    prevBattleState = false;
    phase = ViewPhase::ENVIRONMENT;

    bgSetPriority(0, 2); // set 3D view on main to be behind text layer

    lineSpacing = textSub->getLineSpacing();
}

ViewState EnvironmentView::update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    bgUpdate();
    oamUpdate(&oamSub);

    switch (phase)
    {
    case ViewPhase::BATTLE:
    {
        if (!prevBattleState)
        {
            prevBattleState = true;

            ae::BroadcastEvent(Event::HideAllScreens{});
            ae::BroadcastEvent(Event::ShowMenu{battleMenuCmpt});

            movement->stop();
            ae::BroadcastEvent(Event::StopCamera{});

            startBattle();
        }

        if (!BattleSystem::GetInstance().IsActive() && prevBattleState)
        {
            prevBattleState = false;

            ae::BroadcastEvent(Event::ShowScreen{menuHUDScreen});
            ae::BroadcastEvent(Event::HideAllMenus{});

            prevEnvironmentState = true;

            movement->start();
            ae::BroadcastEvent(Event::StartCamera{});

            phase = ViewPhase::ENVIRONMENT;

            setupMusic();
        }

        break;
    }

    case ViewPhase::PAUSE:
    {
        if (!prevPauseState)
        {
            movement->stop();
            ae::BroadcastEvent(Event::StopCamera{});

            prevPauseState = true;

            ae::BroadcastEvent(Event::HideAllScreens{});
            ae::BroadcastEvent(Event::ShowMenu{pauseMenuCmpt});
        }

        if ((systemKeysDown & KEY_START) || pauseMenuCmpt->isClosed)
        {
            prevPauseState = false;
            textSub->clearScreen();

            ae::BroadcastEvent(Event::HideAllMenus{});

            prevEnvironmentState = false;

            movement->start();
            ae::BroadcastEvent(Event::StartCamera{});

            phase = ViewPhase::ENVIRONMENT;
        }

        break;
    }

    case ViewPhase::DIALOGUE:
    {
        bool isActive = dialogue->IsActive();

        if (!isActive && !prevDialogueState)
        {
            ae::BroadcastEvent(Event::ShowScreen{dialogueScreen});

            movement->stop();
            ae::BroadcastEvent(Event::StopCamera{});

            if (dialogueFirstLine != nullptr)
            {
                dialogue->start(dialogueFirstLine);
            }

            prevDialogueState = true;
        }
        else if (!isActive && prevDialogueState)
        {
            ae::BroadcastEvent(Event::HideAllScreens{});

            prevDialogueState = false;
            prevEnvironmentState = false;

            movement->start();
            ae::BroadcastEvent(Event::StartCamera{});

            phase = ViewPhase::ENVIRONMENT;
        }

        break;
    }

    case ViewPhase::ENVIRONMENT:
    {
        if (!prevEnvironmentState)
        {
            ae::BroadcastEvent(Event::ShowScreen{menuHUDScreen});
            prevEnvironmentState = true;
        }

        CharacterPosition charPos = movement->isCharacterAt();
        camPos = CameraSystem::GetInstance().getCameraPosition();

        if (systemKeysDown & KEY_START)
        {
            textSub->clearScreen();
            prevEnvironmentState = false;

            movement->stop();
            ae::BroadcastEvent(Event::StopCamera{});

            phase = ViewPhase::PAUSE;
            break;
        }

        if (systemKeysDown & KEY_TOUCH)
        {
            touchRead(&touch);

            if (menuHUDScreen->onTouch(&touch) == 1)
            {
                prevEnvironmentState = false;

                movement->stop();
                ae::BroadcastEvent(Event::StopCamera{});

                phase = ViewPhase::PAUSE;
                break;
            }
        }

        ViewState tileResult = onTileCheck(movement->isTileAt(), systemKeysDown);

        if (tileResult != ViewState::KEEP_CURRENT)
        {
            movement->stop();
            ae::BroadcastEvent(Event::StopCamera{});

            musicCtrl->pause();
            return tileResult;
        }

        gluLookAtf32(camPos.eye.x.raw_value(),
                     (camPos.eye.y + getCameraYOffset()).raw_value(),
                     camPos.eye.z.raw_value(),
                     camPos.target.x.raw_value(),
                     camPos.target.y.raw_value(),
                     camPos.target.z.raw_value(),
                     camPos.up.x.raw_value(),
                     camPos.up.y.raw_value(),
                     camPos.up.z.raw_value());

        // environment
        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG | POLY_ID(0));
        env.draw();
        env.drawBillboards(Globals::enableBillboards, camPos.eye.x, camPos.eye.y, camPos.eye.z);
        glPopMatrix(1);

        // model
        glPushMatrix();

        glTranslatef32(charPos.x.raw_value(), charPos.y.raw_value(), charPos.z.raw_value());
        glRotatef(static_cast<float>(charPos.facingAngle), 0.0f, 1.0f, 0.0f);
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG | POLY_ID(1));
        animationCtrl->render();
        glPopMatrix(1);

        glFlush(0);

        if (Globals::enableDebugPrint)
        {
            if (frame % 60 == 30) // restricting this 2Hz otherwise it tanks performance
            {
                textSub->clearArea(1, 120, 128, 72);
                char buf[128];
                std::string debugText = "";
                std::sprintf(buf, "Touch x = %04X, %04X\n", touch.rawx, touch.px);
                debugText += buf;
                std::sprintf(buf, "Touch y = %04X, %04X\n", touch.rawy, touch.py);
                debugText += buf;
                std::sprintf(buf,
                             "tile(x,z): %d, %d\n",
                             (int)(MathManager::GetInstance().div(charPos.x + dbEntry->worldOffsetX, tileSize)),
                             (int)(MathManager::GetInstance().div(charPos.z + dbEntry->worldOffsetZ, tileSize)));
                debugText += buf;
                std::sprintf(buf, "translate(x,z): %d, %d\n", (int)(charPos.x * 100), (int)(charPos.z * 100));
                debugText += buf;
                std::sprintf(buf,
                             "angle(w,c): %d, %d\n",
                             (int)(CameraSystem::GetInstance().getAngle() * 100),
                             (int)(charPos.facingAngle * 100));
                debugText += buf;

                // screen height - 5 lines * (size of text in each line + spacing between each line)
                textSub->drawText(debugText, 1, 192 - 5 * (fontSize + lineSpacing), TextColor::Red);
            }
        }

        break;
    }

    default:
    {
        phase = ViewPhase::ENVIRONMENT;
        break;
    }
    }

    animationCtrl->update();
    musicCtrl->update();

    return ViewState::KEEP_CURRENT;
}

void EnvironmentView::cleanup()
{
    cleanupHook();

    // entity
    if (environment != nullptr)
    {
        engine.DestroyEntity(environment);

        environment = nullptr;
        graphics = nullptr;
        text = nullptr;
        textSub = nullptr;
        textSubAlt = nullptr;
    }

    // entity
    if (player != nullptr)
    {
        engine.DestroyComponent(movement);
        engine.DestroyComponent(dialogue);

        movement = nullptr;
        dialogue = nullptr;
    }

    dialogueScreen = nullptr;
    menuHUDScreen = nullptr;
    battleMenuCmpt = nullptr;
    pauseMenuCmpt = nullptr;

    dbEntry = nullptr;

    dialogueFirstLine = nullptr;

    musicCtrl->cleanup();
    animationCtrl->unloadTextures();
    animationCtrl->stop();

    env.cleanup();

    BaseView::cleanup();
}
