#include "VmContext.hpp"
#include "types/VmTypes.hpp"

using VmNativeFunc = VmNativeResult (*)(VmContext&);

struct VmNativeDesc
{
    VmNativeFunc func;
    uint8_t paramCount;
    bool returnsValue;
};

VmNativeResult Native_SET_DAY(VmContext& ctx);
VmNativeResult Native_GET_DAY(VmContext& ctx);

static const VmNativeDesc gVmNatives[] = {
    {Native_SET_DAY, 1, false},
    {Native_GET_DAY, 0, true},
};
static const size_t gVmNativesCount = sizeof(gVmNatives) / sizeof(gVmNatives[0]);
