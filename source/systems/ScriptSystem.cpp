#include "ScriptSystem.hpp"
#include "CameraSystem.hpp"
#include "amx.h"
#include "types/CameraTypes.hpp"
#include <cstdint>
#include <cstring>
#include <expected>

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

static cell AMX_NATIVE_CALL native_GET_CAMARA_MODE(AMX* amx, const cell params[])
{
    return static_cast<cell>(CameraSystem::getInstance().getMode());
}

static cell AMX_NATIVE_CALL native_SET_CAMARA_MODE(AMX* amx, const cell params[])
{
    CameraMode mode = static_cast<CameraMode>(params[1]);
    logMessage(LogSeverity::DEBUG, "Setting camara to: " + std::to_string(params[1]));
    ae::broadcastEvent(Event::SetCameraMode{mode});
    return CONTINUE;
}

AMX_NATIVE nativeFuncs[] = {native_WAIT, native_GET_CAMARA_MODE, native_SET_CAMARA_MODE};

void ScriptSystem::init()
{
    unloadScript();
    std::string scriptPath = "fat:/data/scripts/example_event.amx";
    if (auto res = loadScript(scriptPath); res.has_value())
    {
        runScript();
        active = true;
    }
}

std::expected<void, uint32_t> ScriptSystem::loadScript(const std::string& scriptPath)
{
    FileBuffer buffer = io.openFileBuffer(scriptPath);
    if (!(buffer.get() != nullptr) || buffer.length() < sizeof(AMX_HEADER))
    {
        logMessage(LogSeverity::ERROR, "Pawn: Failed to open script or file is too small: " + scriptPath);
        lastError = (buffer.get() == nullptr) ? AMX_ERR_NOTFOUND : AMX_ERR_FORMAT;
        status = VmStatus::ERROR;
        return std::unexpected(lastError);
    }

    auto* hdr = reinterpret_cast<AMX_HEADER*>(buffer.get());

    if (hdr->magic != AMX_MAGIC || hdr->stp == 0 || hdr->size >= hdr->stp || hdr->size > buffer.length())
    {
        logMessage(LogSeverity::ERROR, "Pawn: Invalid script format in: " + scriptPath);
        lastError = AMX_ERR_FORMAT;
        status = VmStatus::ERROR;
        return std::unexpected(AMX_ERR_FORMAT);
    }

    if (hdr->stp > MAX_MEMORY_USAGE)
    {
        logMessage(LogSeverity::ERROR,
                   "Pawn: Script requires " + std::to_string(hdr->stp) + " bytes, exceeds MAX_MEMORY_USAGE (" +
                       std::to_string(MAX_MEMORY_USAGE) + "): " + scriptPath);
        lastError = AMX_ERR_MEMORY;
        status = VmStatus::ERROR;
        return std::unexpected(AMX_ERR_MEMORY);
    }

    if (status != VmStatus::UNINITIALIZED)
    {
        unloadScript();
    }

    std::memcpy(vmMemory.data(), buffer.get(), hdr->size);
    int result = amx_Init(&amx, &vmMemory);

    if (result != AMX_ERR_NONE)
    {
        logMessage(LogSeverity::ERROR,
                   "Pawn: amx_Init failed for '" + scriptPath + "' with error: " + std::to_string(result));

        lastError = result;
        status = VmStatus::ERROR;
        unloadScript();

        return std::unexpected(result);
    }

    currentScript = scriptPath;
    status = VmStatus::READY;
    lastError = AMX_ERR_NONE;

    return {};
}

void ScriptSystem::unloadScript()
{
    if (amx.base != nullptr)
    {
        amx_Cleanup(&amx);
        amx.base = nullptr;
    }

    std::memset(&amx, 0, sizeof(amx));
    vmMemory.fill(0);
    currentScript.clear();

    status = VmStatus::UNINITIALIZED;
}

std::expected<void, uint32_t> ScriptSystem::runScript()
{
    status = VmStatus::RUNNING;

    cell ret = 0;
    int execErr = amx_Exec(&amx, &ret, AMX_EXEC_MAIN);
    switch (execErr)
    {
    case AMX_ERR_NONE:
        status = VmStatus::STOPPED;
        return {};
    case AMX_ERR_YIELD:
        status = VmStatus::YIELDED;
        return {};
    case AMX_ERR_SLEEP:
        status = VmStatus::SLEEPING;
        return {};
    default:
        logMessage(LogSeverity::ERROR, "amx_Exec failed with error: " + std::to_string(execErr));
        status = VmStatus::ERROR;
        lastError = execErr;
        return std::unexpected(execErr);
    }

    return {};
}

std::expected<void, uint32_t> ScriptSystem::reloadScript()
{
    if (currentScript.empty())
    {
        logMessage(LogSeverity::WARNING, "Pawn: No script currently loaded to reload");
        return std::unexpected(AMX_ERR_INIT);
    }
    return loadScript(currentScript);
}

void ScriptSystem::update(ae::q20_12_t /*dt*/)
{
    if (!isRunning())
    {
        return;
    }

    cell ret = 0;
    int execErr = amx_Exec(&amx, &ret, AMX_EXEC_CONT);

    switch (execErr)
    {
    case AMX_ERR_YIELD:
        status = VmStatus::YIELDED;
        return;
    case AMX_ERR_SLEEP:
        status = VmStatus::SLEEPING;
        return;
    default:
        break;
    }

    if (execErr != AMX_ERR_NONE)
    {
        logMessage(LogSeverity::ERROR, "amx_Exec failed with error: " + std::to_string(execErr));
        lastError = execErr;
        status = VmStatus::ERROR;
        return;
    }

    logMessage(LogSeverity::DEBUG, "Pawn: Script execution finished");
    status = VmStatus::STOPPED;
}

void ScriptSystem::shutdown()

{
    unloadScript();
}

bool ScriptSystem::canScriptFit(const std::string& scriptPath) const
{
    AMX_HEADER hdr;

    io.readFile(&hdr, scriptPath);

    size_t required = (hdr.magic == AMX_MAGIC) ? static_cast<size_t>(hdr.stp) : 0;

    if (required == 0 || required > MAX_MEMORY_USAGE)
    {
        logMessage(LogSeverity::WARNING,
                   "Script requires " + std::to_string(required) + " bytes, buffer is " +
                       std::to_string(MAX_MEMORY_USAGE));
        return false;
    }

    return true;
}
