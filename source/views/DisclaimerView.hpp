#pragma once
#include "core/globals.hpp"
#include "views/BaseView.hpp"
#include <nds.h>

#include "components/GraphicsComponent.hpp"

class DisclaimerView : public BaseView
{
  private:
    int bg[2];

    ae::Entity* disclaimer = nullptr;
    GraphicsComponent* graphics = nullptr;

  public:
    // override tells compiler we intend to override a virtual fn in a base class (i.e. View)
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
