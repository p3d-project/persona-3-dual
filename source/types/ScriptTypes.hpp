/**
 * @file ScriptTypes.hpp
 * @brief Type definitions, status enumerations, and error codes for the ScriptingSystem.
 * @author Joey Luu (Nothing)
 */

#pragma once

#include <amx.h>
#include <cstdint>
#include <string_view>

/**
 * @brief Execution status of the Pawn virtual machine.
 */
enum class VmStatus : uint8_t
{
    UNINITIALIZED, ///< The VM is uninitialized or has been reset; no script has been loaded.
    READY,         ///< A script is loaded and initialized in memory, ready for execution.
    RUNNING,       ///< The VM is currently executing the loaded script
    SLEEPING, ///< The Script has been put to sleep stopping execution until it is resumed. Similar to Stopped except with the expectation that it will resume.
    YIELDED,  ///< Script has yielded execution back to the engine until the next frame.
    STOPPED,  ///< Script execution has been explicitly stopped.
    FINISHED, ///< Script execution completed successfully.
    ERROR,    ///< An unrecoverable error occurred.
};

/**
 * @brief Unified error codes for the ScriptingSystem
 *
 * Encompasses both the system errors (codes 1-99) and
 * underlying Pawn AMX virtual machine interpreter errors (codes 100+).
 */
enum class ScriptError : uint8_t
{
    None = 0, ///< No error occurred.

    // System Lifecycle & Asset Errors (1 - 99)
    FileNotFound = 1,     ///< Script file could not be found or opened from disk.
    FileTooSmall = 2,     ///< Script file size is smaller than the required AMX header.
    InvalidHeader = 3,    ///< File corrupted or contains invalid AMX header metadata.
    ExceedsMaxMemory = 4, ///< Script required memory exceeds the static VM memory buffer limit.
    NotLoaded = 5,        ///< Attempted an operation requiring a loaded script when none is loaded.
    NoScriptToReload = 6, ///< Cannot reload because no previous script path was recorded.
    NotResumable = 7,     ///< Script is not in a resumable state (must be SLEEPING, STOPPED, or YIELDED).
    FunctionNotFound = 8, ///< Exported public function was not found in the script.
    VariableNotFound = 9, ///< Exported public variable was not found in the script.

    // Pawn VM Interpreter Errors (100+)
    VmExit = 100 + AMX_ERR_EXIT,                   ///< Forced script termination (101).
    VmAssert = 100 + AMX_ERR_ASSERT,               ///< Script assertion check failed (102).
    VmStackCollision = 100 + AMX_ERR_STACKERR,     ///< Stack and heap collided; out of VM memory (103).
    VmBounds = 100 + AMX_ERR_BOUNDS,               ///< Array index out of bounds check failed (104).
    VmMemAccess = 100 + AMX_ERR_MEMACCESS,         ///< Invalid or unauthorized memory access (105).
    VmInvalidInstruction = 100 + AMX_ERR_INVINSTR, ///< Unrecognized or invalid VM bytecode instruction (106).
    VmStackUnderflow = 100 + AMX_ERR_STACKLOW,     ///< Stack underflow occurred (107).
    VmHeapUnderflow = 100 + AMX_ERR_HEAPLOW,       ///< Heap underflow occurred (108).
    VmCallback = 100 + AMX_ERR_CALLBACK,           ///< Error occurred during native callback dispatch (109).
    VmNativeFailed = 100 + AMX_ERR_NATIVE,         ///< Native function execution failed or reported error (110).
    VmDivideByZero = 100 + AMX_ERR_DIVIDE,         ///< Division or modulo by zero (111).
    VmSleep = 100 + AMX_ERR_SLEEP,                 ///< Execution paused by sleep instruction (112).
    VmInvalidState = 100 + AMX_ERR_INVSTATE,       ///< VM entered an invalid internal state (113).
    VmOutOfMemory = 100 + AMX_ERR_MEMORY,          ///< Dynamic memory allocation failed in VM (116).
    VmBadFormat = 100 + AMX_ERR_FORMAT,            ///< Unsupported or incompatible P-code format (117).
    VmVersionMismatch = 100 + AMX_ERR_VERSION,     ///< Incompatible P-code / AMX core version (118).
    VmSymbolNotFound = 100 + AMX_ERR_NOTFOUND,     ///< Required symbol could not be found (119).
    VmBadIndex = 100 + AMX_ERR_INDEX,              ///< Invalid public entry point index (120).
    VmDebug = 100 + AMX_ERR_DEBUG,                 ///< Debugger hook or breakpoint error (121).
    VmNotInitialized = 100 + AMX_ERR_INIT,         ///< AMX instance was not initialized before execution (122).
    VmUserData = 100 + AMX_ERR_USERDATA,           ///< User data table limit reached (123).
    VmInitJit = 100 + AMX_ERR_INIT_JIT,            ///< JIT compiler initialization failure (124).
    VmParams = 100 + AMX_ERR_PARAMS,               ///< Parameter error passed to native or public function (125).
    VmDomain = 100 + AMX_ERR_DOMAIN,               ///< Domain error in mathematical calculation (126).
    VmGeneral = 100 + AMX_ERR_GENERAL,             ///< Unclassified general VM runtime error (127).
    VmOverlay = 100 + AMX_ERR_OVERLAY,             ///< Overlay section load/execution error (128).
    VmYield = 100 + AMX_ERR_YIELD,                 ///< Script yielded execution to engine frame loop (129).
};

/**
 * @brief Checks if an error code originated from the Pawn virtual machine interpreter.
 *
 * @param err The script error to check.
 * @return true if the error code is 100 or greater (Pawn VM interpreter error), false otherwise.
 */
[[nodiscard]] constexpr bool isVmError(ScriptError err) noexcept
{
    return static_cast<uint8_t>(err) >= 100;
}

/**
 * @brief Translates an AMX error code to a strongly-typed ScriptError.
 *
 * @param amxErr Raw error code from the Pawn AMX runtime (e.g. AMX_ERR_*).
 * @return Corresponding @ref ScriptError enum value.
 */
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

/**
 * @brief Converts a ScriptError into a human-readable descriptive string.
 *
 * @param err The script error to convert.
 * @return String view containing a human-readable description of the error.
 */
[[nodiscard]] constexpr std::string_view toString(ScriptError err) noexcept
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
