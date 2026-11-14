/**
 * @file ScriptManager.hpp
 * @brief Manager for loading and running scripts.
 * @author nothing
 */

#pragma once

#include "core/logger.hpp"
#include "etl/array.h"
#include "etl/span.h"
#include "managers/IOManager.hpp"
#include <aegis/manager.hpp>
#include <amx.h>
#include <amxaux.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <uchar.h>

enum class VmStatus : uint8_t
{
    STOPPED,
    RUNNING,
    YIELD,
    ERROR,
};

class ScriptManager : public ae::Manager, public ae::Singleton<ScriptManager>
{
  public:
    void Init() override
    {
        memset(&amx, 0, sizeof(amx));

        std::string scriptPath = "fat:/data/scripts/example_event.amx";

        int err = aux_LoadProgram(&amx, scriptPath.c_str(), vmMemory.data());
        logMessage(LogSeverity::DEBUG, "aux_LoadProgram result: " + std::to_string(err));

        if (err != AMX_ERR_NONE)
        {
            logMessage(LogSeverity::ERROR, "Pawn: Failed to load script: " + scriptPath);
            return;
        }

        const AMX_NATIVE_INFO abc[] = {{"print", print}, {"printf", n_printf}, {0, 0}};

        int regErr = amx_Register(&amx, abc, -1);
        logMessage(LogSeverity::DEBUG, "amx_Register result: " + std::to_string(regErr));

        cell ret = 0;
        int execErr = amx_Exec(&amx, &ret, AMX_EXEC_MAIN);
        logMessage(LogSeverity::DEBUG, "amx_Exec result: " + std::to_string(execErr) + ", ret: " + std::to_string(ret));
    }

    void Process() override
    {
    }

    void Shutdown() override
    {
        if (amx.base != nullptr)
        {
            amx_Cleanup(&amx);
            amx.base = nullptr;
        }
        memset(&amx, 0, sizeof(amx));
    }

    bool loadScript(const std::string& scriptPath);
    bool reloadScript();
    bool unloadScript();
    bool runScript();
    bool resumeScript();
    void stopScript();
    void pauseScript();

    [[nodiscard]]
    bool canScriptFit(const std::string& scriptPath) const;

    [[nodiscard]]
    bool hasPublic(std::string_view funcName) const;
    int callPublic(std::string_view funcName);
    int callPublic(std::string_view funcName, etl::span<const cell> params, cell* retVal = nullptr);

  private:
    friend class Singleton<ScriptManager>;
    ScriptManager() = default;

    static constexpr size_t MAX_MEMORY_USAGE = 6400 * sizeof(cell);

    //FIXME: REMOVE AND REPLACE WITH BETTER LOGGING
    static cell AMX_NATIVE_CALL print(AMX* amx, const cell params[])
    {
        char buffer[256];
        cell* cptr = amx_Address(amx, params[1]);
        amx_GetString(buffer, cptr, 0, sizeof(buffer));
        logMessage(LogSeverity::DEBUG, std::string("[Pawn print] ") + buffer);
        return 0;
    }

    //FIXME: REMOVE AND REPLACE WITH BETTER LOGGING
    static cell AMX_NATIVE_CALL n_printf(AMX* amx, const cell params[])
    {
        char buffer[256];
        cell* cptr = amx_Address(amx, params[1]);
        amx_GetString(buffer, cptr, 0, sizeof(buffer));
        logMessage(LogSeverity::DEBUG, std::string("[Pawn printf] ") + buffer);
        return 0;
    }

    VmStatus status = VmStatus::STOPPED;
    std::string currrentScript;

    AMX amx{};
    alignas(sizeof(cell)) inline static etl::array<uint8_t, MAX_MEMORY_USAGE> vmMemory;
    // static constexpr uint16_t amx_flags = (AMX_FLAG_SLEEP | AMX_FLAG_DEBUG);

    IOManager& io = IOManager::GetInstance();
};
