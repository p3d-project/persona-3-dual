#pragma once

#include "VmStack.hpp"
#include <types/VmTypes.hpp>

struct VmContext
{
    const VmInstruction* script = nullptr;
    VmStatus status = VmStatus::STOPPED;
    uint32_t pc;
    uint16_t steps;

    VmStack stack;
    bool nativeCallPending = false;
    uint16_t nativeCallArgsIndex = 0;

    VmValue nativeCallReturnValue;
    VmStackType nativeCallReturnType;

    void reset(const VmInstruction* newScript)
    {
        script = newScript;
        status = VmStatus::STOPPED;
        pc = 0;
        steps = 0;
        nativeCallPending = false;
        nativeCallArgsIndex = 0;
    }

    void setNativeCallReturnValue(VmValue value, VmStackType type)
    {
        nativeCallReturnValue = value;
        nativeCallReturnType = type;
    }
};
