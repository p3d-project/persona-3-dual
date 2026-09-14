#pragma once
#include "components/MusicComponent.hpp"
#include "views/BaseView.hpp"

class DebugView : public BaseView
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;

  private:
    ae::Entity* debug = nullptr;
    MusicComponent* music = nullptr;
};
