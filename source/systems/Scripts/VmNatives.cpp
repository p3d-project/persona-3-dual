#include "VmNatives.hpp"
#include "components/screens/MenuHUDScreen.hpp"
#include "core/logger.hpp"

static int32_t getIntParam(VmContext& ctx, size_t index)
{
    // don't need to be too strict about type checking here, recast to int32_t if it's a float, since the VM is not strongly typed
    if (ctx.stack.topType(index) != VmStackType::INTEGER)
    {
        logMessage(LogSeverity::ERROR, "VM: Expected integer argument on stack for native call");
        return 0;
    }

    return ctx.stack.top(index).i;
}

static ae::q20_12_t getFloatParam(VmContext& ctx, size_t index)
{
    // same as above
    if (ctx.stack.topType(index) != VmStackType::FLOAT)
    {
        logMessage(LogSeverity::ERROR, "VM: Expected float argument on stack for native call");
        return ae::q20_12_t(0);
    }

    return ctx.stack.top(index).q;
}

static inline void setIntReturnValue(VmContext& ctx, int32_t value)
{
    ctx.setNativeCallReturnValue({.i = value}, VmStackType::INTEGER);
}

static inline void setFloatReturnValue(VmContext& ctx, ae::q20_12_t value)
{
    ctx.setNativeCallReturnValue({.q = value}, VmStackType::FLOAT);
}

VmNativeResult Native_SET_DAY(VmContext& ctx)
{
    int32_t day = getIntParam(ctx, 0);

    if (day < 1 || day > 31)
    {
        logMessage(LogSeverity::ERROR, "VM: Invalid day value for SET_DAY");
        return VmNativeResult::DONE;
    }

    auto menuHUDScreen = MenuHUDScreen::getInstance();
    menuHUDScreen->day = day;

    return VmNativeResult::DONE;
}

VmNativeResult Native_GET_DAY(VmContext& ctx)
{
    auto menuHUDScreen = MenuHUDScreen::getInstance();
    int32_t day = menuHUDScreen->day;

    setIntReturnValue(ctx, day);

    return VmNativeResult::DONE;
}
