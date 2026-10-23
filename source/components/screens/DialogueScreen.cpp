#include "DialogueScreen.hpp"
#include "core/globals.hpp"
#include "core/logger.hpp"
#include "managers/MathManager.hpp"
#include "uiAnimation/UIEase.hpp"

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
               srs.y + panelOffset,
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

void DialogueScreen::showAnimated()
{
    panelAnimator.cancelAll();
    panelOffset = 40;
    panelAnimation = panelAnimator.animate(panelOffset).from(40).to(0).duration(220).ease(UIEase::OutCubic).start();
}

void DialogueScreen::hideAnimated()
{
    panelAnimator.cancelAll();
    panelAnimation =
        panelAnimator.animate(panelOffset).from(panelOffset).to(40).duration(180).ease(UIEase::InCubic).start();
}

void DialogueScreen::updateAnimation()
{
    bool panelWasAnimating = panelAnimation.isPlaying();
    bool bustWasAnimating = bustAnimation.isPlaying();
    const ae::q20_12_t dt = MathManager::GetInstance().div(ae::q20_12_t{1}, ae::q20_12_t{60});
    panelAnimator.update(dt);
    bustAnimator.update(dt);

    if (isLoaded && (panelWasAnimating || bustWasAnimating || panelAnimation.isPlaying() || bustAnimation.isPlaying()))
    {
        renderSprites();
        updateBustPositions();
    }
}

bool DialogueScreen::isAnimating() const
{
    return panelAnimation.isPlaying();
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
    if (bustPayloads == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Called with null bustPayloads");
        return;
    }

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
    activeBust = &bustPayload;
    bustAnimator.cancelAll();
    bustOffsetX = 0;
    auto focus = bustAnimator.sequence();
    focus.append(bustAnimator.animate(bustOffsetX).from(0).to(-3).duration(50).ease(UIEase::OutQuad));
    focus.append(bustAnimator.animate(bustOffsetX).from(-3).to(3).duration(90).ease(UIEase::InOutQuad));
    focus.append(bustAnimator.animate(bustOffsetX).from(3).to(0).duration(70).ease(UIEase::OutQuad));
    bustAnimation = focus.start();

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
               srs.x + bustOffsetX,
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

void DialogueScreen::updateBustPositions()
{
    if (activeBust == nullptr)
    {
        return;
    }

    int bustId = spriteId;
    for (SpritePayload& sp : *activeBust)
    {
        Sprite& sprite = sp.srs.sprite;
        if (sprite.gfx == nullptr || sp.ga.id <= -1)
        {
            continue;
        }
        oamSetXY(oam, bustId++, sp.srs.x + bustOffsetX, sp.srs.y);
    }
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
    panelAnimator.cancelAll();
    bustAnimator.cancelAll();
    panelAnimation = UIAnimationHandle();
    bustAnimation = UIAnimationHandle();
    panelOffset = 40;
    bustOffsetX = 0;
    activeBust = nullptr;

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
