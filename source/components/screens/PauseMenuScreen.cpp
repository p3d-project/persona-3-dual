#include "PauseMenuScreen.hpp"
#include "core/globals.hpp"

PauseMenuScreen* PauseMenuScreen::instance = nullptr;

void PauseMenuScreen::create()
{
    if (instance == nullptr)
    {
        instance = new PauseMenuScreen();
    }
}

void PauseMenuScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

PauseMenuScreen* PauseMenuScreen::getInstance()
{
    if (instance == nullptr)
    {
        instance = new PauseMenuScreen();
    }
    return instance;
}

void PauseMenuScreen::setYenText(const std::string& yen, TextColor color)
{
    yenText = yen;
    yenColor = color;
}

void PauseMenuScreen::setPersonaText(const std::string& persona, TextColor color)
{
    personaText = persona;
    personaColor = color;
}

void PauseMenuScreen::setArcanaText(const std::string& arcana, TextColor color)
{
    arcanaText = arcana;
    arcanaColor = color;
}

void PauseMenuScreen::setLevelText(const std::string& level, TextColor color)
{
    levelText = level;
    levelColor = color;
}

void PauseMenuScreen::setDescriptionText(const std::string& description, TextColor color)
{
    descriptionText = description;
    descriptionColor = color;
}

void PauseMenuScreen::loadBackground()
{
    // load background into ram
    bgPauseMenu = graphics->loadGraphic(bgPath + "pause/pause");
}

void PauseMenuScreen::renderBackground()
{
    // load palette
    dmaCopy(bgPauseMenu.pal, BG_PALETTE_SUB, bgPauseMenu.palLen);

    // draw background (copy into vram)
    dmaCopy(bgPauseMenu.tiles, bgGetGfxPtr(bgId), bgPauseMenu.tilesLen);
    dmaCopy(bgPauseMenu.map, bgGetMapPtr(bgId), bgPauseMenu.mapLen);
}

void PauseMenuScreen::unloadBackground()
{
    // unload background from ram
    graphics->unloadGraphic(bgPauseMenu);
    bgPauseMenu = {};
}

void PauseMenuScreen::renderText()
{
    // description (top bar)
    text->clearArea(82, 5, 166, 14);
    text->drawText(descriptionText, 72, 8, descriptionColor);

    text->clearArea(138, 92, 82, 70);

    // yen
    text->drawText(yenText, 138, 92, yenColor);

    //  active persona name
    text->drawText(personaText, 138, 117, personaColor);

    // active persona arcana
    text->drawText(arcanaText, 138, 136, arcanaColor);

    // active persona level
    text->drawText(levelText, 138, 155, levelColor);

    renderOptions();
}

void PauseMenuScreen::renderOptions()
{
    etl::span<MenuOption> options = menu->getOptions();
    int startIndex = menu->getStartIndex();
    int selectedOption = menu->getSelectedOption();

    for (int i = 0; i < visibleOptions && startIndex + i < int(options.size()); i++)
    {
        int option = startIndex + i;
        TextColor color = option == selectedOption ? selectedOptionColor : optionColor;
        text->drawText(options[option].name, optionsStartX, optionsStartY + optionsRowHeight * i, color);
    }

    int y = optionsStartY + optionsRowHeight * (selectedOption - startIndex) + selectedBarYOffset;

    etl::array<SpriteRenderState*, 4> barPieces = {&srs1, &srs2, &srs3, &srs4};
    for (int i = 0; i < 4; i++)
    {
        barPieces[i]->y = y;
        if (isLoaded)
        {
            moveSprite(i + 1, barPieces[i]->x, y);
        }
    }
}

void PauseMenuScreen::renderSprites()
{
    // load palettes
    dmaCopy(bgPauseMenu.pal, SPRITE_PALETTE_SUB, bgPauseMenu.palLen);

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

int PauseMenuScreen::onTouch(touchPosition* touch)
{
    if (touch->px >= 193 && touch->px <= 250 && touch->py >= 166 && touch->py <= 184)
    {
        return 1;
    }

    return -1;
}

void PauseMenuScreen::setTextComponent(TextComponent* text)
{
    this->text = text;
}

void PauseMenuScreen::setMenu(const UIMenu* menu)
{
    this->menu = menu;
}

int PauseMenuScreen::getVisibleOptions()
{
    return visibleOptions;
}

void PauseMenuScreen::load()
{
    // create relevant entities, components
    if (pauseMenu == nullptr)
    {
        pauseMenu = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        pauseMenu->AddComponent(graphics);
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

void PauseMenuScreen::unload()
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

    if (pauseMenu != nullptr)
    {
        engine.DestroyEntity(pauseMenu);

        pauseMenu = nullptr;
        graphics = nullptr;
    }
}
