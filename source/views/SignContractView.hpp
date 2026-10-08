#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/MusicComponent.hpp"
#include "components/SFXComponent.hpp"
#include "components/TextComponent.hpp"
#include "components/screens/SignContractScreen.hpp"
#include "views/BaseView.hpp"

#include <maxmod9.h>

class SignContractView : public BaseView
{
  private:
    int bg;

    ae::Entity* signContract = nullptr;

    SignContractScreen* signContractScreen = nullptr;
    GraphicsComponent* graphics = nullptr;

    MusicComponent* musicCmpt = nullptr;
    SFXComponent* sfxCmpt = nullptr;

    std::array<int, 2> bgMain;
    std::array<int, 3> bgSub;

    void cancelSFX();

    touchPosition touch;

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
