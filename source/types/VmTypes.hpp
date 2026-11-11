#pragma once

#include <aegis/types.hpp>

constexpr int STACK_BYTES = 1024;
constexpr int MAX_LOCALS_COUNT = 16;
constexpr int MAX_STACK_SIZE = STACK_BYTES / 8;

enum class VmOpcode : uint16_t
{
    CALL_NATIVE,
    PUSH_INT,
    PUSH_FLOAT,
    ADD,
    END
};

enum class VmStackType : uint8_t
{
    INTEGER,
    FLOAT,
};

enum class VmStatus : uint8_t
{
    STOPPED,
    RUNNING,
    YIELD,
    ERROR,
};

enum class VmStepReturn : uint8_t
{
    NEXT_INSTRUCTION,
    YIELD,
    STOP
};

enum class VmNativeResult : uint8_t
{
    DONE,
    PENDING
};

union VmValue
{
    int32_t i;
    ae::q20_12_t q;
};

struct VmInstruction
{
    VmOpcode opcode;
    uint16_t operand;
};
