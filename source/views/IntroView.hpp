#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"
#include "controllers/MusicController.hpp"
#include "managers/MathManager.hpp"
#include "managers/UIManager.hpp"
#include "views/BaseView.hpp"

#include <etl/array.h>

class IntroView : public BaseView
{
  private:
    Sprite logoSprite[2];
    // 64
    SpriteRenderState srs0 = {logoSprite[0], 5, 128};
    SpriteRenderState srs1 = {logoSprite[1], 69, 128};
    etl::array<SpriteRenderState, 2> spriteRenderStates = {srs0, srs1};

    int bg[4];

    // sub screen
    int bgSubLogo;
    int bgSubSky;
    int bgSubText;
    uint16_t* textVideoBufferSub;

    // for silhouette animation
    int silhouetteX = -256;
    int silhouetteY = 192;

    // for bottom screen text animation
    bool animateText = false;
    int duration = 4;
    int durationCounter = 0;
    int textAlpha = 0;
    int textAlphaDirection = 0;

    // for logoSprite
    bool displayLogo = false;
    int logoOpacity = 0;

    // for overlayBackground
    bool displayOverlay = false;
    int overlayOpacity = 0;
    // NOTE: we use u16 to allow overflow (and naturally reset values back to 0)
    u16 waveAngle = 0;
    u16 currentRotation = 0;
    int baseSpeed = 20;
    int fluctuation = 50;

    // text
    std::string FONT_NAME = "cosmetica";
    int FONT_SIZE = 24;

    ae::Entity* intro = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;

    MathManager& math = MathManager::GetInstance();
    UIManager& ui = UIManager::GetInstance();
    MusicController* musicCtrl = MusicController::getInstance();

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
