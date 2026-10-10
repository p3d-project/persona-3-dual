#include "PauseMenuHUDScreen.hpp"
#include "core/globals.hpp"

PauseMenuHUDScreen* PauseMenuHUDScreen::instance = nullptr;

void PauseMenuHUDScreen::create()
{
    if (instance == nullptr)
    {
        instance = new PauseMenuHUDScreen();
    }
}

void PauseMenuHUDScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

PauseMenuHUDScreen* PauseMenuHUDScreen::getInstance()
{
    if (instance == nullptr)
    {
        instance = new PauseMenuHUDScreen();
    }
    return instance;
}

void PauseMenuHUDScreen::loadBackground()
{
    // load background into ram
    bgHUD = graphics->loadGraphic(bgPath + "menuHUD/menuHUD");
}

void PauseMenuHUDScreen::renderBackground()
{
    // load palettes
    vramSetBankH(VRAM_H_LCD);
    dmaCopy(bgHUD.pal, &VRAM_H_EXT_PALETTE[bgId % 4][0], bgHUD.palLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    // draw background (copy into vram)
    dmaCopy(bgHUD.tiles, bgGetGfxPtr(bgId), bgHUD.tilesLen);
    dmaCopy(bgHUD.map, bgGetMapPtr(bgId), bgHUD.mapLen);
}

void PauseMenuHUDScreen::unloadBackground()
{
    // unload background from ram
    graphics->unloadGraphic(bgHUD);
    bgHUD = {};
}

void PauseMenuHUDScreen::renderSprites()
{
    // NOTE: we are currently assuming that the sprite extended palette will be set on VRAM bank I
    // TODO: remove extended palette, and use normal palette sprites

    // load palettes
    int k = 0;
    vramSetBankI(VRAM_I_LCD);
    for (GraphicAsset*& ga : spritePalettes)
    {
        if (ga != nullptr)
        {
            dmaCopy(ga->pal, &VRAM_I_EXT_SPR_PALETTE[k][0], ga->palLen);
        }
        k++;
    }
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    // perform transformations
    /// index -1 is reserved for vflip/hflip, 0 is reserved for no transform
    int i = 1;
    for (SpriteTransform& st : spriteTransforms)
    {
        oamRotateScale(oam, i++, st.angle, st.sx, st.sy);
    }

    // draw sprites
    int j = 0;
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;
        SpriteRenderState& srs = sp.srs;

        // copy sprite into vram
        dmaCopy(graphic.tiles, sprite.gfx, graphic.tilesLen);

        oamSet(oam,
               j++,
               srs.x,
               srs.y,
               srs.priority,
               srs.sprite.paletteAlpha,
               srs.sprite.size,
               srs.sprite.format,
               srs.sprite.gfx,
               srs.affineIndex,
               srs.sizeDouble,
               srs.hide,
               srs.hflip,
               srs.vflip,
               srs.mosaic);
    }

    // draw background
    renderBackground();
}

int PauseMenuHUDScreen::onTouch(touchPosition* touch)
{
    if (touch->px >= 193 && touch->px <= 250 && touch->py >= 166 && touch->py <= 184)
    {
        return 1;
    }

    return -1;
}

void PauseMenuHUDScreen::load()
{
    // create relevant entities, components
    if (pauseMenuHUD == nullptr)
    {
        pauseMenuHUD = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        pauseMenuHUD->AddComponent(graphics);
    }

    // load sprites
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // load graphic if not already loaded
        if (graphic.id <= -1)
        {
            // allocating space for sprite
            sprite.gfx = oamAllocateGfx(oam, sprite.size, sprite.format);

            // load sprite into ram
            graphic = graphics->loadSpriteGraphic(std::string(sp.spritePath), sp.spriteType, sp.spriteVariant);
        }
    }

    // load background
    loadBackground();
};

void PauseMenuHUDScreen::unload()
{
    // hide sprites
    removeSprites();

    // free sprites
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // free sprite vram
        if (sprite.gfx != nullptr)
        {
            oamFreeGfx(oam, sprite.gfx);
            sprite.gfx = nullptr;
        }

        // unload graphic if not already unloaded
        if (graphic.id > -1)
        {
            // unload sprite
            graphics->unloadGraphic(graphic);

            // reset data
            graphic = {};
        }
    }

    // unload background
    unloadBackground();

    if (pauseMenuHUD != nullptr)
    {
        engine.DestroyEntity(pauseMenuHUD);

        pauseMenuHUD = nullptr;
        graphics = nullptr;
    }
}
