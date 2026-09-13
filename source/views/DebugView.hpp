#pragma once
#include "views/BaseView.hpp"

class DebugView : public BaseView
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
