#pragma once
#include "types/StateTypes.hpp"

class BaseView
{
  public:
    // the destructor (runs when we delete a view)
    // viritual is used for overriding functions (polymorphism)
    // in this case, it allows child classes instances (ex. IntroView) to be properly cleaned up when referred throguh the parent (View) class
    virtual ~BaseView() = default;

    // abstract methods, as marked by "= 0"
    virtual void init() = 0;        // view setup
    virtual ViewState update() = 0; // view update
    virtual void cleanup();         // view cleanup
};
