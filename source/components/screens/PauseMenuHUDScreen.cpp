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

void PauseMenuHUDScreen::setInfoPanelText(const std::string& yen,
                                          const std::string& persona,
                                          const std::string& arcana,
                                          const std::string& level,
                                          TextColor color)
{
    yenText = yen;
    personaText = persona;
    arcanaText = arcana;
    levelText = level;
    textColor = color;
}

void PauseMenuHUDScreen::setDescriptionText(const std::string& description, TextColor color)
{
    descriptionText = description;
    descriptionColor = color;
}

void PauseMenuHUDScreen::loadBackground()
{
    // load background into ram
    bgHUD = graphics->loadGraphic(bgPath + "pause/pause");
}

void PauseMenuHUDScreen::renderBackground()
{
    // load palette
    dmaCopy(bgHUD.pal, BG_PALETTE_SUB, bgHUD.palLen);

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

void PauseMenuHUDScreen::renderText()
{
    // description (top bar)
    text->clearArea(82, 5, 166, 14);
    text->drawText(descriptionText, 72, 8, descriptionColor);

    text->clearArea(138, 92, 82, 70);

    // yen
    text->drawText(yenText, 138, 92, textColor);

    //  active persona name
    text->drawText(personaText, 138, 117, textColor);

    // active persona arcana
    text->drawText(arcanaText, 138, 136, textColor);

    // active persona level
    text->drawText(levelText, 138, 155, textColor);
}

void PauseMenuHUDScreen::renderSprites()
{
    // load palettes
    // dmaCopy(bgHUD.pal, SPRITE_PALETTE_SUB, bgHUD.palLen);
    dmaCopy(statusGraphic.pal, SPRITE_PALETTE_SUB, statusGraphic.palLen);

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

    // draw text
    renderText();
}

int PauseMenuHUDScreen::onTouch(touchPosition* touch)
{
    if (touch->px >= 193 && touch->px <= 250 && touch->py >= 166 && touch->py <= 184)
    {
        return 1;
    }

    return -1;
}

void PauseMenuHUDScreen::setTextComponent(TextComponent* text)
{
    this->text = text;
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
