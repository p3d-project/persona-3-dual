#include "ScriptingSystem.hpp"
#include "CameraSystem.hpp"
#include "amx.h"
#include "types/CameraTypes.hpp"
#include <cstdint>
#include <cstring>
#include <expected>
#include <ranges>
#include <utility>

#define YIELD amx_RaiseError(amx, AMX_ERR_YIELD)
#define CONTINUE AMX_ERR_NONE

static cell AMX_NATIVE_CALL native_WAIT(AMX* amx, const cell params[])
{
    static int waitCounter = 0;
    cell waitingFrames = params[1];

    if (waitCounter < waitingFrames)
    {
        waitCounter++;
        return YIELD;
    }

    waitCounter = 0;
    return CONTINUE;
}

static cell AMX_NATIVE_CALL native_GET_CAMERA_MODE(AMX* amx, const cell params[])
{
    return static_cast<cell>(CameraSystem::getInstance().getMode());
}

static cell AMX_NATIVE_CALL native_SET_CAMERA_MODE(AMX* amx, const cell params[])
{
    CameraMode mode = static_cast<CameraMode>(params[1]);
    logMessage(LogSeverity::DEBUG, "Setting camara to: " + std::to_string(params[1]));
    ae::broadcastEvent(Event::SetCameraMode{mode});
    return CONTINUE;
}

AMX_NATIVE nativeFuncs[] = {native_WAIT, native_GET_CAMERA_MODE, native_SET_CAMERA_MODE};

void ScriptingSystem::init()
{
    reset();
    std::string scriptPath = "fat:/data/scripts/example_event.amx";
    if (auto res = loadScript(scriptPath); res.has_value())
    {
        if (runScript().has_value())
        {
            logMessage(LogSeverity::DEBUG, "Successful loading of Script");
        }
    }
}

std::expected<void, ScriptError> ScriptingSystem::loadScript(const std::string& scriptPath)
{
    FileBuffer buffer = io.openFileBuffer(scriptPath);
    if (buffer.get() == nullptr)
    {
        logMessage(LogSeverity::ERROR, "Pawn: Failed to open script: " + scriptPath);
        lastError = ScriptError::FileNotFound;
        setStatus(VmStatus::ERROR);
        return std::unexpected(lastError);
    }

    if (buffer.length() < sizeof(AMX_HEADER))
    {
        logMessage(LogSeverity::ERROR, "Pawn: Script file too small: " + scriptPath);
        lastError = ScriptError::FileTooSmall;
        setStatus(VmStatus::ERROR);
        return std::unexpected(lastError);
    }

    // This should be safe if the buffer is aligned
    // FIXME: Make sure that is true or not
    auto* hdr = reinterpret_cast<AMX_HEADER*>(buffer.get());

    if (hdr->magic != AMX_MAGIC || hdr->stp == 0 || hdr->size < static_cast<uint32_t>(sizeof(AMX_HEADER)) ||
        hdr->size >= hdr->stp || static_cast<size_t>(hdr->size) > buffer.length())
    {
        logMessage(LogSeverity::ERROR, "Pawn: Invalid script format in: " + scriptPath);
        lastError = ScriptError::InvalidHeader;
        setStatus(VmStatus::ERROR);
        return std::unexpected(lastError);
    }

    if (std::cmp_greater(hdr->stp, MAX_MEMORY_USAGE))
    {
        logMessage(LogSeverity::ERROR,
                   "Pawn: Script requires " + std::to_string(hdr->stp) + " bytes, exceeds MAX_MEMORY_USAGE (" +
                       std::to_string(MAX_MEMORY_USAGE) + "): " + scriptPath);
        lastError = ScriptError::ExceedsMaxMemory;
        setStatus(VmStatus::ERROR);
        return std::unexpected(lastError);
    }

    if (status != VmStatus::UNINITIALIZED)
    {
        reset();
    }

    std::memcpy(vmMemory.data(), buffer.get(), hdr->size);
    int result = amx_Init(&amx, vmMemory.data());

    if (result != AMX_ERR_NONE)
    {
        lastError = toScriptError(result);
        logMessage(LogSeverity::ERROR,
                   "Pawn: amx_Init failed for '" + scriptPath + "' with error: " + toString(lastError));

        reset();
        setStatus(VmStatus::ERROR);

        return std::unexpected(lastError);
    }

    currentScript = scriptPath;
    setStatus(VmStatus::READY);
    lastError = ScriptError::None;

    return {};
}

void ScriptingSystem::reset()
{
    if (status != VmStatus::UNINITIALIZED)
    {
        amx_Cleanup(&amx);
        amx.base = nullptr;
    }

    std::memset(&amx, 0, sizeof(amx));
    vmMemory.fill(0);
    currentScript.clear();

    setStatus(VmStatus::UNINITIALIZED);
}

// TODO: Take a single argument for running a public function based on name
std::expected<void, ScriptError> ScriptingSystem::runScript()
{
    if (!isLoaded())
    {
        lastError = ScriptError::NotLoaded;
        return std::unexpected(lastError);
    }

    return ScriptingSystem::execute(AMX_EXEC_MAIN);
}

std::expected<void, ScriptError> ScriptingSystem::execute(int execIndex)
{
    setStatus(VmStatus::RUNNING);

    cell ret = 0;
    const int execErr = amx_Exec(&amx, &ret, execIndex);

    switch (execErr)
    {
    case AMX_ERR_NONE:
        logMessage(LogSeverity::DEBUG, "Pawn: Script execution finished");
        setStatus(VmStatus::FINISHED);
        lastError = ScriptError::None;
        return {};
    case AMX_ERR_YIELD:
        setStatus(VmStatus::YIELDED);
        lastError = ScriptError::None;
        return {};
    case AMX_ERR_SLEEP:
        setStatus(VmStatus::SLEEPING);
        lastError = ScriptError::None;
        return {};
    default:
        lastError = toScriptError(execErr);
        logMessage(LogSeverity::ERROR, "Pawn: amx_Exec failed with error: " + std::string(toString(lastError)));
        setStatus(VmStatus::ERROR);
        return std::unexpected(lastError);
    }
}

std::expected<void, ScriptError> ScriptingSystem::reloadScript()
{
    if (currentScript.empty())
    {
        logMessage(LogSeverity::WARNING, "Pawn: No script currently loaded to reload");
        lastError = ScriptError::NoScriptToReload;
        return std::unexpected(lastError);
    }

    std::string scriptCopy = currentScript;
    return loadScript(scriptCopy);
}

void ScriptingSystem::update(ae::q20_12_t /*dt*/)
{
    if (isRunning())
    {
        auto res = resumeScript();
        if (!res.has_value())
        {
        }

        return;
    }
}

void ScriptingSystem::shutdown()

{
    reset();
}

bool ScriptingSystem::canScriptFit(const std::string& scriptPath) const
{
    AMX_HEADER hdr{};

    // This is safe and won't overflow the stack as readFile only reads the file to the size of AMX_HEADER and no more.
    if (!io.readFile(&hdr, scriptPath))
    {
        return false;
    }

    size_t required = (hdr.magic == AMX_MAGIC) ? static_cast<size_t>(hdr.stp) : 0;

    if (required <= 0 || required > MAX_MEMORY_USAGE)
    {
        logMessage(LogSeverity::WARNING,
                   "Script requires " + std::to_string(required) + " bytes, buffer is " +
                       std::to_string(MAX_MEMORY_USAGE));
        return false;
    }

    return true;
}

const cell* ScriptingSystem::getPublicVariableAddress(const std::string& varName) const
{
    if (!isLoaded())
    {
        return nullptr;
    }
    if (varName.empty() || varName.length() > sNAMEMAX)
    {
        return nullptr;
    }

    cell* address = nullptr;
    if (amx_FindPubVar(const_cast<AMX*>(&amx), varName.c_str(), &address) == AMX_ERR_NONE)
    {
        return address;
    }

    return nullptr;
}

cell* ScriptingSystem::getPublicVariableAddress(const std::string& varName)
{
    return const_cast<cell*>(std::as_const(*this).getPublicVariableAddress(varName));
}

bool ScriptingSystem::hasPublicVariable(const std::string& varName) const
{
    return getPublicVariableAddress(varName) != nullptr;
}

std::expected<cell, ScriptError> ScriptingSystem::getPublicVariable(const std::string& varName) const
{
    if (!isLoaded())
    {
        return std::unexpected(ScriptError::NotLoaded);
    }

    const cell* address = getPublicVariableAddress(varName);
    if (address == nullptr)
    {
        return std::unexpected(ScriptError::VariableNotFound);
    }

    return *address;
}

std::expected<void, ScriptError> ScriptingSystem::setPublicVariable(const std::string& varName, cell value)
{
    if (!isLoaded())
    {
        lastError = ScriptError::NotLoaded;
        return std::unexpected(lastError);
    }

    cell* address = getPublicVariableAddress(varName);
    if (address == nullptr)
    {
        lastError = ScriptError::VariableNotFound;
        return std::unexpected(lastError);
    }

    *address = value;
    return {};
}

bool ScriptingSystem::hasPublic(const std::string& funcName) const
{
    if (!isLoaded() || funcName.empty() || funcName.length() > sNAMEMAX)
    {
        return false;
    }

    int index = 0;
    return amx_FindPublic(const_cast<AMX*>(&amx), funcName.c_str(), &index) == AMX_ERR_NONE;
}

std::expected<cell, ScriptError> ScriptingSystem::callPublic(const std::string& funcName, etl::span<const cell> params)
{
    if (!isLoaded())
    {
        lastError = ScriptError::NotLoaded;
        return std::unexpected(lastError);
    }

    int index = 0;
    int err = amx_FindPublic(&amx, funcName.c_str(), &index);
    if (err != AMX_ERR_NONE)
    {
        lastError = ScriptError::FunctionNotFound;
        return std::unexpected(lastError);
    }

    // Pawn calling convention requires parameters in reverse order
    for (cell param : std::views::reverse(params))
    {
        err = amx_Push(&amx, param);
        if (err != AMX_ERR_NONE)
        {
            lastError = toScriptError(err);
            return std::unexpected(lastError);
        }
    }

    cell result = 0;
    err = amx_Exec(&amx, &result, index);
    if (err == AMX_ERR_NONE)
    {
        lastError = ScriptError::None;
        return {result};
    }

    lastError = toScriptError(err);
    return std::unexpected(lastError);
}

std::expected<void, ScriptError> ScriptingSystem::resumeScript()
{
    if (!isResumable())
    {
        logMessage(LogSeverity::WARNING, "Pawn: Cannot resume script from current state");
        lastError = ScriptError::NotResumable;
        return std::unexpected(lastError);
    }

    return ScriptingSystem::execute(AMX_EXEC_CONT);
}
void ScriptingSystem::stopScript()
{
    if (isRunning() || isSleeping())
    {
        setStatus(VmStatus::STOPPED);
    }
}
void ScriptingSystem::pauseScript()
{
    if (isRunning())
    {
        setStatus(VmStatus::SLEEPING);
    }
}

void ScriptingSystem::setStatus(VmStatus newStatus)
{
    status = newStatus;
    switch (status)
    {
        using enum VmStatus;
    case RUNNING:
    case YIELDED:
        setActive(true);
        break;
    case FINISHED:
    case UNINITIALIZED:
    case READY:
    case SLEEPING:
    case STOPPED:
    default:
        setActive(false);
        break;
    }
}
