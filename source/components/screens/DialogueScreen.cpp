#include "DialogueScreen.hpp"
#include "core/globals.hpp"

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

void DialogueScreen::renderSprites()
{
    // load palettes
    dmaCopy(palette0, SPRITE_PALETTE_SUB, 16 * sizeof(u16));

    // perform transformations
    /// index -1 is reserved for vflip/hflip, 0 is reserved for no transform
    int i = 1;
    for (SpriteTransform& st : spriteTransforms)
    {
        oamRotateScale(oam, i++, st.angle, st.sx, st.sy);
    }

    // draw sprites
    spriteId = 0;
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;
        SpriteRenderState& srs = sp.srs;

        // copy sprite into vram
        dmaCopy(graphic.tiles, sprite.gfx, graphic.tilesLen);

        oamSet(oam,
               spriteId++,
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
}

void DialogueScreen::load()
{
    // create relevant entities, components
    if (dialogue == nullptr)
    {
        dialogue = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        dialogue->AddComponent(graphics);
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

    // set palettes
    palette0 = blueBlockGraphic.pal;
    palette1 = (graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE_GREEN)).pal;
};

void DialogueScreen::loadBusts(etl::array<etl::span<SpritePayload>, 10>* bustPayloads)
{
    for (etl::span<SpritePayload>& sp : *bustPayloads)
    {
        loadBust(sp);
    }
    this->bustPayloads = bustPayloads;
    prevBust = nullptr;
}

void DialogueScreen::loadBust(etl::span<SpritePayload>& bustPayload)
{
    // load new bust
    for (SpritePayload& sp : bustPayload)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // setup sprite
        sprite.paletteAlpha = bustPaletteId;

        // load graphic if not already loaded
        if (graphic.id <= -1)
        {
            // allocating space for sprite
            sprite.gfx = oamAllocateGfx(oam, sprite.size, sprite.format);

            // load sprite into ram
            graphic = graphics->loadSpriteGraphic(sp.spritePath, sp.spriteType, sp.spriteVariant);
        }
    }
}

void DialogueScreen::renderBust(etl::span<SpritePayload>& bustPayload)
{
    // exit if no bust
    if (bustPayload.empty())
    {
        return;
    }
    // don't re-render the bust
    else if (prevBust != nullptr && (prevBust->data() == bustPayload.data()))
    {
        return;
    }
    // hide the old bust sprites
    else if (prevBust != nullptr)
    {
        // hide sprites
        oamClear(oam, spriteId, prevBust->size());

        // clear palette
        dmaFillHalfWords(0, SPRITE_PALETTE_SUB + (bustPaletteId * 16), 16 * sizeof(u16));
    }

    // update to new bust
    prevBust = &bustPayload;

    // draw bust
    int bustId = spriteId;
    void* bustPalette = nullptr;
    for (SpritePayload& sp : bustPayload)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;
        SpriteRenderState& srs = sp.srs;

        // skip; graphics have not been loaded
        if (sprite.gfx == nullptr || graphic.id <= -1)
        {
            continue;
        }

        // save the latest palette
        bustPalette = graphic.pal;

        // copy sprite into vram
        dmaCopy(graphic.tiles, sprite.gfx, graphic.tilesLen);

        oamSet(oam,
               bustId++,
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

    // setup bust palette
    dmaCopy(bustPalette, SPRITE_PALETTE_SUB + (bustPaletteId * 16), 16 * sizeof(u16));
}

void DialogueScreen::unloadBust(etl::span<SpritePayload>& bustPayload)
{
    for (SpritePayload& sp : bustPayload)
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
            sprite.paletteAlpha = -1;
            graphic = {};
        }
    }
}

void DialogueScreen::unload()
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

    // free bust sprites
    if (bustPayloads != nullptr)
    {
        for (etl::span<SpritePayload>& sp : *bustPayloads)
        {
            unloadBust(sp);
        }

        // reset array pointers
        bustPayloads = nullptr;
        prevBust = nullptr;
    }

    if (dialogue != nullptr)
    {
        engine.DestroyEntity(dialogue);

        dialogue = nullptr;
        graphics = nullptr;
    }
}

void DialogueScreen::triggerAction(UIAction action)
{
    switch (action)
    {
    // default palette
    case UIAction::SwitchToPalette0:
    {
        dmaCopy(palette0, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
        break;
    }

    // option selection palette
    case UIAction::SwitchToPalette1:
    {
        dmaCopy(palette1, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
        break;
    }

    default:
    {
        break;
    }
    }
}
