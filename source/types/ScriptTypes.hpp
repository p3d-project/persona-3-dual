#pragma once

#include <amx.h>
#include <cstdint>

enum class VmStatus : uint8_t
{
    UNINITIALIZED,
    READY,
    RUNNING,
    SLEEPING,
    YIELDED,
    STOPPED,
    FINISHED,
    ERROR,
};

enum class ScriptError : uint8_t
{
    None = 0,

    // System Lifecycle & Asset Errors (1 - 99)
    FileNotFound = 1,
    FileTooSmall = 2,
    InvalidHeader = 3,
    ExceedsMaxMemory = 4,
    NotLoaded = 5,
    NoScriptToReload = 6,
    NotResumable = 7,
    FunctionNotFound = 8,
    VariableNotFound = 9,

    // Pawn VM Interpreter Errors (100+)
    VmExit = 100 + AMX_ERR_EXIT,                   // 101
    VmAssert = 100 + AMX_ERR_ASSERT,               // 102
    VmStackCollision = 100 + AMX_ERR_STACKERR,     // 103
    VmBounds = 100 + AMX_ERR_BOUNDS,               // 104
    VmMemAccess = 100 + AMX_ERR_MEMACCESS,         // 105
    VmInvalidInstruction = 100 + AMX_ERR_INVINSTR, // 106
    VmStackUnderflow = 100 + AMX_ERR_STACKLOW,     // 107
    VmHeapUnderflow = 100 + AMX_ERR_HEAPLOW,       // 108
    VmCallback = 100 + AMX_ERR_CALLBACK,           // 109
    VmNativeFailed = 100 + AMX_ERR_NATIVE,         // 110
    VmDivideByZero = 100 + AMX_ERR_DIVIDE,         // 111
    VmSleep = 100 + AMX_ERR_SLEEP,                 // 112
    VmInvalidState = 100 + AMX_ERR_INVSTATE,       // 113
    VmOutOfMemory = 100 + AMX_ERR_MEMORY,          // 116
    VmBadFormat = 100 + AMX_ERR_FORMAT,            // 117
    VmVersionMismatch = 100 + AMX_ERR_VERSION,     // 118
    VmSymbolNotFound = 100 + AMX_ERR_NOTFOUND,     // 119
    VmBadIndex = 100 + AMX_ERR_INDEX,              // 120
    VmDebug = 100 + AMX_ERR_DEBUG,                 // 121
    VmNotInitialized = 100 + AMX_ERR_INIT,         // 122
    VmUserData = 100 + AMX_ERR_USERDATA,           // 123
    VmInitJit = 100 + AMX_ERR_INIT_JIT,            // 124
    VmParams = 100 + AMX_ERR_PARAMS,               // 125
    VmDomain = 100 + AMX_ERR_DOMAIN,               // 126
    VmGeneral = 100 + AMX_ERR_GENERAL,             // 127
    VmOverlay = 100 + AMX_ERR_OVERLAY,             // 128
    VmYield = 100 + AMX_ERR_YIELD,                 // 129
};

using PawnError = ScriptError;

[[nodiscard]] constexpr bool isVmError(ScriptError err) noexcept
{
    return static_cast<uint8_t>(err) >= 100;
}

[[nodiscard]] constexpr ScriptError toScriptError(int amxErr) noexcept
{
    switch (amxErr)
    {
    case AMX_ERR_NONE:
        return ScriptError::None;
    case AMX_ERR_EXIT:
        return ScriptError::VmExit;
    case AMX_ERR_ASSERT:
        return ScriptError::VmAssert;
    case AMX_ERR_STACKERR:
        return ScriptError::VmStackCollision;
    case AMX_ERR_BOUNDS:
        return ScriptError::VmBounds;
    case AMX_ERR_MEMACCESS:
        return ScriptError::VmMemAccess;
    case AMX_ERR_INVINSTR:
        return ScriptError::VmInvalidInstruction;
    case AMX_ERR_STACKLOW:
        return ScriptError::VmStackUnderflow;
    case AMX_ERR_HEAPLOW:
        return ScriptError::VmHeapUnderflow;
    case AMX_ERR_CALLBACK:
        return ScriptError::VmCallback;
    case AMX_ERR_NATIVE:
        return ScriptError::VmNativeFailed;
    case AMX_ERR_DIVIDE:
        return ScriptError::VmDivideByZero;
    case AMX_ERR_SLEEP:
        return ScriptError::VmSleep;
    case AMX_ERR_INVSTATE:
        return ScriptError::VmInvalidState;
    case AMX_ERR_MEMORY:
        return ScriptError::VmOutOfMemory;
    case AMX_ERR_FORMAT:
        return ScriptError::VmBadFormat;
    case AMX_ERR_VERSION:
        return ScriptError::VmVersionMismatch;
    case AMX_ERR_NOTFOUND:
        return ScriptError::VmSymbolNotFound;
    case AMX_ERR_INDEX:
        return ScriptError::VmBadIndex;
    case AMX_ERR_DEBUG:
        return ScriptError::VmDebug;
    case AMX_ERR_INIT:
        return ScriptError::VmNotInitialized;
    case AMX_ERR_USERDATA:
        return ScriptError::VmUserData;
    case AMX_ERR_INIT_JIT:
        return ScriptError::VmInitJit;
    case AMX_ERR_PARAMS:
        return ScriptError::VmParams;
    case AMX_ERR_DOMAIN:
        return ScriptError::VmDomain;
    case AMX_ERR_GENERAL:
        return ScriptError::VmGeneral;
    case AMX_ERR_OVERLAY:
        return ScriptError::VmOverlay;
    case AMX_ERR_YIELD:
        return ScriptError::VmYield;
    default:
        return ScriptError::VmGeneral;
    }
}

[[nodiscard]] constexpr const char* toString(ScriptError err) noexcept
{
    using enum ScriptError;
    switch (err)
    {
    case None:
        return "None";
    // Engine errors
    case FileNotFound:
        return "Script file not found";
    case FileTooSmall:
        return "Script file too small for AMX header";
    case InvalidHeader:
        return "Invalid or corrupted AMX header";
    case ExceedsMaxMemory:
        return "Script memory exceeds MAX_MEMORY_USAGE";
    case NotLoaded:
        return "No script is currently loaded";
    case NoScriptToReload:
        return "Cannot reload, no script path recorded";
    case NotResumable:
        return "Script is not in a resumable state";
    case FunctionNotFound:
        return "Public function not found";
    case VariableNotFound:
        return "Public variable not found";
    // VM errors
    case VmExit:
        return "VM: Forced exit";
    case VmAssert:
        return "VM: Assertion failed";
    case VmStackCollision:
        return "VM: Stack/heap collision";
    case VmBounds:
        return "VM: Array index out of bounds";
    case VmMemAccess:
        return "VM: Invalid memory access";
    case VmInvalidInstruction:
        return "VM: Invalid instruction";
    case VmStackUnderflow:
        return "VM: Stack underflow";
    case VmHeapUnderflow:
        return "VM: Heap underflow";
    case VmCallback:
        return "VM: Callback error";
    case VmNativeFailed:
        return "VM: Native function failed";
    case VmDivideByZero:
        return "VM: Integer divide by zero";
    case VmSleep:
        return "VM: Sleep mode";
    case VmInvalidState:
        return "VM: Invalid state";
    case VmOutOfMemory:
        return "VM: Out of memory";
    case VmBadFormat:
        return "VM: Unsupported P-code format";
    case VmVersionMismatch:
        return "VM: Incompatible P-code version";
    case VmSymbolNotFound:
        return "VM: Symbol not found";
    case VmBadIndex:
        return "VM: Invalid entry point index";
    case VmDebug:
        return "VM: Debugger error";
    case VmNotInitialized:
        return "VM: Not initialized";
    case VmUserData:
        return "VM: User data table full";
    case VmInitJit:
        return "VM: JIT init error";
    case VmParams:
        return "VM: Parameter error";
    case VmDomain:
        return "VM: Domain error";
    case VmGeneral:
        return "VM: General error";
    case VmOverlay:
        return "VM: Overlay error";
    case VmYield:
        return "VM: Script yielded";
    default:
        return "Unknown script error";
    }
}
