#include "DialogueScreen.h"

DialogueScreen* DialogueScreen::instance = nullptr;

void DialogueScreen::create()
{
    if (instance == nullptr)
    {
        instance = new DialogueScreen();
    }
}

void DialogueScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

DialogueScreen* DialogueScreen::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

// TODO: clean up and properly implement class

// helper
void DialogueScreen::renderSprites()
{
    // NOTE: we are currently assuming that the sprite extended palette will be set on VRAM bank I
    vramSetBankI(VRAM_I_LCD);
    dmaCopy(calendarSprite[0].pal, &VRAM_I_EXT_SPR_PALETTE[0][0], calendarSprite[0].palLen);
    dmaCopy(textBox[0].pal, &VRAM_I_EXT_SPR_PALETTE[1][0], textBox[0].palLen);
    dmaCopy(nameTag[0].pal, &VRAM_I_EXT_SPR_PALETTE[2][0], nameTag[0].palLen);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    int i = 0;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           true,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 1;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 2;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 3;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 4;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 5;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           true,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 6;
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
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    // rotate sprite
    // oamRotateScale(
    //     oam,
    //     sprites[i].rotationIndex,
    //     degreesToAngle(0),
    //     intToFixed(1, 8),
    //     intToFixed(1, 8)
    // );

    // render multiple of the same sprits
    // in this case, the skills level
    // for (int i = 0; i < 3; i++)
    // {
    //     oamSet(
    //         oam,                    // sub display (OamState)
    //         7 + i,                      // oam entry to set (id)
    //         sprites[7].x + (13 * i), sprites[7].y, // position
    //         1,                          // priority
    //         sprites[7].paletteAlpha,    // palette for 16 color sprite or alpha for bmp sprite
    //         sprites[7].size,
    //         sprites[7].format,
    //         sprites[7].gfx,
    //         sprites[7].rotationIndex,
    //         true,         // double the size of rotated sprites
    //         false,        // don't hide the sprite
    //         false, false, // vflip, hflip
    //         false         // apply mosaic
    //     );
    // }
}

void DialogueScreen::removeSprites()
{
    oamClear(oam, 0, 6);
}

void DialogueScreen::load()
{
    if (dialogue == nullptr)
    {
        dialogue = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        dialogue->AddComponent(graphics);
    }

    // load graphics
    std::string spritePath = "graphics/Dialogue/sprites/";

    // setup sprites
    // calendar
    sprites[0] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, -1, 0, 192, 0};
    sprites[1] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 0, 128, 0};
    // text box
    sprites[2] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 0, 128};
    sprites[3] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 64, 128};
    sprites[4] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 128, 128};
    sprites[5] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, -1, 1, 192, 128};
    // name tag
    sprites[6] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 2, 20, 112};

    // allocating space for sprite graphics
    // calendar
    sprites[0].gfx = oamAllocateGfx(oam, sprites[0].size, sprites[0].format);
    sprites[1].gfx = oamAllocateGfx(oam, sprites[1].size, sprites[1].format);
    // text box
    sprites[2].gfx = oamAllocateGfx(oam, sprites[2].size, sprites[2].format);
    sprites[3].gfx = oamAllocateGfx(oam, sprites[3].size, sprites[3].format);
    sprites[4].gfx = oamAllocateGfx(oam, sprites[4].size, sprites[4].format);
    sprites[5].gfx = oamAllocateGfx(oam, sprites[5].size, sprites[5].format);
    // name tag
    sprites[6].gfx = oamAllocateGfx(oam, sprites[6].size, sprites[6].format);

    // get sprites
    // calendar
    calendarSprite[0] = graphics->loadSpriteGraphic(
        spritePath, SpriteType::DIALOGUE, saveData.femcMode ? DialogueSprite::CALENDAR_FEMC : DialogueSprite::CALENDAR);
    calendarSprite[1] = graphics->loadSpriteGraphic(
        spritePath, SpriteType::DIALOGUE, saveData.femcMode ? DialogueSprite::CALENDAR_FEMC : DialogueSprite::CALENDAR);
    // text box
    textBox[0] =
        graphics->loadSpriteGraphic(spritePath,
                                    SpriteType::DIALOGUE,
                                    saveData.femcMode ? DialogueSprite::TEXT_CORNER_FEMC : DialogueSprite::TEXT_CORNER);
    textBox[1] =
        graphics->loadSpriteGraphic(spritePath,
                                    SpriteType::DIALOGUE,
                                    saveData.femcMode ? DialogueSprite::TEXT_MIDDLE_FEMC : DialogueSprite::TEXT_MIDDLE);
    textBox[2] =
        graphics->loadSpriteGraphic(spritePath,
                                    SpriteType::DIALOGUE,
                                    saveData.femcMode ? DialogueSprite::TEXT_MIDDLE_FEMC : DialogueSprite::TEXT_MIDDLE);
    textBox[3] =
        graphics->loadSpriteGraphic(spritePath,
                                    SpriteType::DIALOGUE,
                                    saveData.femcMode ? DialogueSprite::TEXT_CORNER_FEMC : DialogueSprite::TEXT_CORNER);
    // name tag
    nameTag[0] = graphics->loadSpriteGraphic(
        spritePath, SpriteType::DIALOGUE, saveData.femcMode ? DialogueSprite::NAME_TAG_FEMC : DialogueSprite::NAME_TAG);

    // copy sprites into memory
    // calendar
    dmaCopy(calendarSprite[0].tiles, sprites[0].gfx, calendarSprite[0].tilesLen);
    dmaCopy(calendarSprite[1].tiles, sprites[1].gfx, calendarSprite[1].tilesLen);
    // text box
    dmaCopy(textBox[0].tiles, sprites[2].gfx, textBox[0].tilesLen);
    dmaCopy(textBox[1].tiles, sprites[3].gfx, textBox[1].tilesLen);
    dmaCopy(textBox[2].tiles, sprites[4].gfx, textBox[2].tilesLen);
    dmaCopy(textBox[3].tiles, sprites[5].gfx, textBox[3].tilesLen);
    // name tag
    dmaCopy(nameTag[0].tiles, sprites[6].gfx, nameTag[0].tilesLen);
};

void DialogueScreen::unload()
{
    if (graphics != nullptr)
    {
        graphics->unloadAll();
    }

    if (dialogue != nullptr)
    {
        dialogue->RemoveComponent<GraphicsComponent>();
        engine.DestroyEntity(dialogue);

        dialogue = nullptr;
        graphics = nullptr;
    }
}
