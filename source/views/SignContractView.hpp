#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/MusicComponent.hpp"
#include "components/SFXComponent.hpp"
#include "components/TextComponent.hpp"
#include "views/BaseView.hpp"

#include <maxmod9.h>

class SignContractView : public BaseView
{
  private:
    int bg[3];
    bool isLastName = true;
    bool isNameConfirmed = false;
    int lastNameIndex = 0;
    int firstNameIndex = 0;

    // text
    std::string FONT_NAME = "cosmetica";
    int FONT_SIZE = 12;
    std::string animText;
    std::string displayText;

    ae::Entity* signContract = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;

    MusicComponent* musicCmpt = nullptr;
    SFXComponent* sfxCmpt = nullptr;

    void cancelSFX();

  public:
    void SetMusicComponent(MusicComponent* music)
    {
        musicCmpt = music;
    }
    void SetSFXComponent(SFXComponent* sfx)
    {
        sfxCmpt = sfx;
    }

    void init() override;
    ViewState update() override;
    void cleanup() override;
};
