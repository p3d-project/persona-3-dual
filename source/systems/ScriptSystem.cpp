#include "ScriptSystem.hpp"
#include "CameraSystem.hpp"
#include "types/CameraTypes.hpp"

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
    memset(&amx, 0, sizeof(amx));
    vmMemory.fill(0);
    currentScript.clear();

    std::string scriptPath = "fat:/data/scripts/example_event.amx";

    int err = aux_LoadProgram(&amx, scriptPath.c_str(), vmMemory.data());
    logMessage(LogSeverity::DEBUG, "aux_LoadProgram result: " + std::to_string(err));

    if (err != AMX_ERR_NONE)
    {
        logMessage(LogSeverity::ERROR, "Pawn: Failed to load script: " + scriptPath);
        return;
    }

    status = VmStatus::RUNNING;
    cell ret = 0;
    int execErr = amx_Exec(&amx, &ret, AMX_EXEC_MAIN);
}

void ScriptSystem::update(ae::q20_12_t /*dt*/)
{
    if (status == VmStatus::STOPPED || status == VmStatus::ERROR)
    {
        return;
    }
    status = VmStatus::RUNNING;

    cell ret = 0;
    int execErr = amx_Exec(&amx, &ret, AMX_EXEC_CONT);

    if (execErr == AMX_ERR_YIELD)
    {
        status = VmStatus::YIELDED;
        return;
    }
    if (execErr != AMX_ERR_NONE)
    {
        logMessage(LogSeverity::ERROR, "amx_Exec failed with error: " + std::to_string(execErr));
        status = VmStatus::ERROR;
        return;
    }

    logMessage(LogSeverity::DEBUG, "Pawn: Script execution finished");
    status = VmStatus::STOPPED;
}

void ScriptSystem::shutdown()

{
    if (amx.base != nullptr)
    {
        amx_Cleanup(&amx);
        amx.base = nullptr;
    }
    memset(&amx, 0, sizeof(amx));
}

bool ScriptSystem::canScriptFit(const std::string& scriptPath) const
{
    AMX_HEADER hdr;

    io.readFile(&hdr, scriptPath);

    amx_Align16(&hdr.magic);
    amx_Align32(reinterpret_cast<uint32_t*>(&hdr.stp));

    size_t required = (hdr.magic == AMX_MAGIC) ? static_cast<size_t>(hdr.stp) : 0;

    if (required == 0 || required > MAX_MEMORY_USAGE)
    {
        logMessage(LogSeverity::ERROR,
                   "Script requires " + std::to_string(required) + " bytes, buffer is " +
                       std::to_string(MAX_MEMORY_USAGE));
        return false;
    }

    return true;
}
