#include "MenuHUDScreen.hpp"
#include "core/globals.hpp"

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

void MenuHUDScreen::loadBackground()
{
    // load background into ram
    bgHUD = graphics->loadGraphic(bgPath + "menuHUD/menuHUD");
}

void MenuHUDScreen::renderBackground()
{
    // load palettes
    vramSetBankH(VRAM_H_LCD);
    dmaCopy(bgHUD.pal, &VRAM_H_EXT_PALETTE[bgId % 4][0], bgHUD.palLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    // draw background (copy into vram)
    dmaCopy(bgHUD.tiles, bgGetGfxPtr(bgId), bgHUD.tilesLen);
    dmaCopy(bgHUD.map, bgGetMapPtr(bgId), bgHUD.mapLen);
}

void MenuHUDScreen::unloadBackground()
{
    // unload background from ram
    graphics->unloadGraphic(bgHUD);
    bgHUD = {};
}

void MenuHUDScreen::renderSprites()
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

    // draw text
    renderText();
}

int MenuHUDScreen::onTouch(touchPosition* touch)
{
    int xPos = 207;
    int yPos = 173;
    int width = 41;
    int height = 14;

    if (touch->px >= xPos && touch->px <= (xPos + width) && touch->py >= yPos && touch->py <= (yPos + height))
    {
        return 1;
    }

    return -1;
}

void MenuHUDScreen::load()
{
    // create relevant entities, components
    if (menuHUD == nullptr)
    {
        menuHUD = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        menuHUD->AddComponent(graphics);
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
            graphic = graphics->loadSpriteGraphic(sp.spritePath, sp.spriteType, sp.spriteVariant);
        }
    }

    // load background
    loadBackground();
};

void MenuHUDScreen::unload()
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

    if (menuHUD != nullptr)
    {
        engine.DestroyEntity(menuHUD);

        menuHUD = nullptr;
        graphics = nullptr;
    }
}

void MenuHUDScreen::setTextComponent(TextComponent* text)
{
    this->text = text;
}

void MenuHUDScreen::setTextContent(std::string textContent, TextColor textColor)
{
    this->textContent = textContent;
    this->textColor = textColor;
    renderText();
}

void MenuHUDScreen::renderText()
{
    // place
    text->clearArea(3, 3, 134, 19);
    text->drawText(textContent.c_str(), 5, 6, textColor);

    // date
    text->drawText("12/31", 208, 5, TextColor::White);

    // day
    text->drawText("Sa", 243, 5, TextColor::White);

    // days until full moon
    text->drawText("30", 243, 64, TextColor::White);
}
