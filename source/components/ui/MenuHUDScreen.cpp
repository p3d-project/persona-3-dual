#include "MenuHUDScreen.h"

MenuHUDScreen* MenuHUDScreen::instance = nullptr;

void MenuHUDScreen::create()
{
    if (instance == nullptr)
    {
        instance = new MenuHUDScreen();
    }
}

void MenuHUDScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

MenuHUDScreen* MenuHUDScreen::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MenuHUDScreen();
    }
    return instance;
}

// helper
void MenuHUDScreen::loadBackground()
{
    if (bgLoaded)
        return;

    std::string bgPath = "graphics/MenuHUD/backgrounds/";
    GraphicAsset bgHUD =
        graphics->loadGraphic(bgPath + (saveData.femcMode ? "menuHUDFEMC/menuHUDFEMC" : "menuHUD/menuHUD"));

    dmaCopy(bgHUD.tiles, bgGetGfxPtr(bgId), bgHUD.tilesLen);
    dmaCopy(bgHUD.map, bgGetMapPtr(bgId), bgHUD.mapLen);

    vramSetBankH(VRAM_H_LCD);
    dmaCopy(bgHUD.pal, &VRAM_H_EXT_PALETTE[bgId % 4][0], bgHUD.palLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    graphics->unloadGraphic(bgHUD);
    bgLoaded = true;
}

void MenuHUDScreen::renderSprites()
{
    // NOTE: we are currently assuming that the sprite extended palette will be set on VRAM bank I
    vramSetBankI(VRAM_I_LCD);
    dmaCopy(moonSprite.pal, &VRAM_I_EXT_SPR_PALETTE[0][0], moonSprite.palLen);             // moon
    dmaCopy(dayOfWeekSprite.pal, &VRAM_I_EXT_SPR_PALETTE[1][0], dayOfWeekSprite.palLen);   // day of the week
    dmaCopy(numberSprites[0].pal, &VRAM_I_EXT_SPR_PALETTE[2][0], numberSprites[0].palLen); // numbers & slash
    dmaCopy(timeSprites[0].pal, &VRAM_I_EXT_SPR_PALETTE[3][0], timeSprites[0].palLen);     // time (0)
    dmaCopy(timeSprites[1].pal, &VRAM_I_EXT_SPR_PALETTE[4][0], timeSprites[1].palLen);     // time (1)
    dmaCopy(timeSprites[2].pal, &VRAM_I_EXT_SPR_PALETTE[5][0], timeSprites[2].palLen);     // time (2)
    dmaCopy(timeSprites[3].pal, &VRAM_I_EXT_SPR_PALETTE[6][0], timeSprites[3].palLen);     // time (3)
    dmaCopy(skillSprites[0].pal, &VRAM_I_EXT_SPR_PALETTE[7][0], skillSprites[0].palLen);   // skill level
    if (animReady && animAsset[0].pal != NULL && animAsset[0].palLen > 0)
    {
        u16* animPalette = static_cast<u16*>(animAsset[0].pal);
        animPalette[0] = 0;
        dmaCopy(animAsset[0].pal, &VRAM_I_EXT_SPR_PALETTE[8][0], animAsset[0].palLen); // anim frames
    }
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    // draw sprites
    int count = animReady ? kAnimSlot + 1 : 12;
    for (int i = 0; i < count; i++)
    {
        oamSet(oam, // sub display (OamState)
               i,   // oam entry to set (id)
               sprites[i].x,
               sprites[i].y,            // position
               1,                       // priority
               sprites[i].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
               sprites[i].size,
               sprites[i].format,
               sprites[i].gfx,
               sprites[i].rotationIndex,
               true,  // double the size of rotated sprites
               false, // don't hide the sprite
               false,
               false, // vflip, hflip
               false  // apply mosaic
        );
    }
}

void MenuHUDScreen::removeSprites()
{
    oamClear(oam, 0, animReady ? kAnimSlot + 1 : 12);
}

void MenuHUDScreen::tick()
{
    if (!isLoaded || !animReady)
        return;

    int frameIndex = (frame / 6) % kAnimFrames;
    sprites[kAnimSlot].gfx = animGfx[frameIndex];

    if (animAsset[frameIndex].pal != nullptr && animAsset[frameIndex].palLen > 0)
    {
        u16* animPalette = static_cast<u16*>(animAsset[frameIndex].pal);
        animPalette[0] = 0;
        vramSetBankI(VRAM_I_LCD);
        dmaCopy(animAsset[frameIndex].pal, &VRAM_I_EXT_SPR_PALETTE[8][0], animAsset[frameIndex].palLen);
        vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    }

    const int cycle = 180;
    animX = 176 - (96 * (frame % cycle)) / (cycle - 1);
    sprites[kAnimSlot].x = animX;

    const int t = frame % cycle;
    const int half = cycle / 2;
    const int ramp = (t < half) ? t : (cycle - 1 - t);
    const int scale = 256 - (85 * ramp) / half; // 256 down to 171 and back
    oamRotateScale(oam, kAnimAffine, 0, scale, scale);

    oamSet(oam,
           kAnimSlot,
           sprites[kAnimSlot].x,
           sprites[kAnimSlot].y,
           0,
           sprites[kAnimSlot].paletteAlpha,
           sprites[kAnimSlot].size,
           sprites[kAnimSlot].format,
           sprites[kAnimSlot].gfx,
           kAnimAffine,
           true,
           false,
           false,
           false,
           false);
}

int MenuHUDScreen::onTouch(touchPosition* touch)
{
    if (touch->px >= 193 && touch->px <= 250 && touch->py >= 166 && touch->py <= 184)
    {
        return 1;
    }

    return -1;
}

void MenuHUDScreen::load()
{
    if (menuHUD == nullptr)
    {
        menuHUD = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        menuHUD->AddComponent(graphics);
    }

    // load graphics
    bgLoaded = false;
    std::string spritePath = "graphics/MenuHUD/sprites/";

    // setup sprites
    // moon
    sprites[0] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 202, -15};
    // day of the week
    sprites[1] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 1, 134, 143};
    // numbers
    sprites[2] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, -11, 141}; // month (10s)
    sprites[3] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 15, 141};  // month (1s)
    sprites[4] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 54, 141};  // day (10s)
    sprites[5] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 80, 141};  // day (1s)
    // time
    sprites[6] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 3, -27, -5}; // piece 0
    sprites[7] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 4, 37, -5};  // piece 1
    sprites[8] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 5, 101, -5}; // piece 2
    sprites[9] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 6, 165, -5}; // piece 3
    // skill level
    sprites[10] = {0, SpriteSize_16x16, SpriteColorFormat_256Color, 0, 7, 90, 77};
    // slash
    sprites[11] = {0, SpriteSize_16x16, SpriteColorFormat_256Color, 0, 2, 52, 157};

    // allocating space for sprite graphics
    //moon
    sprites[0].gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    // day of the week
    sprites[1].gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    // numbers
    sprites[2].gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites[3].gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites[4].gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites[5].gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    // time
    sprites[6].gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites[7].gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites[8].gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites[9].gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    // skill level
    sprites[10].gfx = oamAllocateGfx(oam, SpriteSize_16x16, SpriteColorFormat_256Color);
    // slash
    sprites[11].gfx = oamAllocateGfx(oam, SpriteSize_16x16, SpriteColorFormat_256Color);

    sprites[kAnimSlot] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, kAnimAffine, 8, animX, animY};
    animReady = false;
    for (int i = 0; i < kAnimFrames; i++)
        animGfx[i] = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);

    // get sprites
    // moon
    moonSprite = graphics->loadSpriteGraphic(spritePath, SpriteType::MOON, MoonSprite::MOON_22);
    // day of the week
    dayOfWeekSprite = graphics->loadSpriteGraphic(spritePath, SpriteType::DAY_OF_WEEK, DayOfWeekSprite::TUESDAY);
    // numbers
    numberSprites[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_0);
    numberSprites[1] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_4);
    numberSprites[2] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_0);
    numberSprites[3] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_7);
    // time
    timeSprites[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_0_0);
    timeSprites[1] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_1_0);
    timeSprites[2] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_2_0);
    timeSprites[3] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_3_0);
    // skill level
    skillSprites[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::SKILL_SPRITE, SkillSprite::SKILLS_LEVEL);
    // slash
    slashSprite = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::SLASH);

    // TODO: initialize any extra sprite registers for max-case arrays?
    // ...

    // copy sprites into memory
    // moon
    dmaCopy(moonSprite.tiles, sprites[0].gfx, moonSprite.tilesLen);
    // day of the week
    dmaCopy(dayOfWeekSprite.tiles, sprites[1].gfx, dayOfWeekSprite.tilesLen);
    // numbers
    dmaCopy(numberSprites[0].tiles, sprites[2].gfx, numberSprites[0].tilesLen);
    dmaCopy(numberSprites[1].tiles, sprites[3].gfx, numberSprites[1].tilesLen);
    dmaCopy(numberSprites[2].tiles, sprites[4].gfx, numberSprites[2].tilesLen);
    dmaCopy(numberSprites[3].tiles, sprites[5].gfx, numberSprites[3].tilesLen);
    // time
    dmaCopy(timeSprites[0].tiles, sprites[6].gfx, timeSprites[0].tilesLen);
    dmaCopy(timeSprites[1].tiles, sprites[7].gfx, timeSprites[1].tilesLen);
    dmaCopy(timeSprites[2].tiles, sprites[8].gfx, timeSprites[2].tilesLen);
    dmaCopy(timeSprites[3].tiles, sprites[9].gfx, timeSprites[3].tilesLen);
    // skill level
    dmaCopy(skillSprites[0].tiles, sprites[10].gfx, skillSprites[0].tilesLen);
    // slash
    dmaCopy(slashSprite.tiles, sprites[11].gfx, slashSprite.tilesLen);

    const char* names[kAnimFrames] = {
        "tile064",
        "tile065",
        "tile066",
        "tile067",
        "tile068",
        "tile069",
    };

    bool ok = true;
    for (int i = 0; i < kAnimFrames; ++i)
    {
        std::string framePath = "graphics/MenuHUD/sprites/" + std::string(names[i]) + "/" + std::string(names[i]);
        animAsset[i] = graphics->loadGraphic(framePath);
        if (animAsset[i].pal != nullptr && animAsset[i].palLen >= 2)
        {
            u16* palette = static_cast<u16*>(animAsset[i].pal);
            palette[0] = 0;
        }
        if (animGfx[i] && animAsset[i].tiles && animAsset[i].tilesLen >= 1024)
        {
            dmaCopy(animAsset[i].tiles, animGfx[i], 1024);
        }
        else
        {
            ok = false;
        }
    }

    if (ok)
    {
        sprites[kAnimSlot].gfx = animGfx[0];
        animReady = true;
    }

    loadBackground();
}

void MenuHUDScreen::unload()
{
    for (int i = 0; i < kAnimFrames; ++i)
    {
        if (animAsset[i].tiles != nullptr || animAsset[i].pal != nullptr || animAsset[i].map != nullptr)
        {
            graphics->unloadGraphic(animAsset[i]);
        }
        animAsset[i] = GraphicAsset{};
        animGfx[i] = nullptr;
    }
    animReady = false;

    if (graphics != nullptr)
    {
        graphics->unloadAll();
    }

    if (menuHUD != nullptr)
    {
        menuHUD->RemoveComponent<GraphicsComponent>();
        engine.DestroyEntity(menuHUD);

        menuHUD = nullptr;
        graphics = nullptr;
    }
}
