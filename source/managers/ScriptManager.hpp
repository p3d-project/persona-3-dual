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
    UNINITIALIZED,
    READY,
    RUNNING,
    SLEEPING,
    YIELDED,
    STOPPED,
    FINISHED,
    ERROR,
};

class ScriptManager : public ae::Manager, public ae::Singleton<ScriptManager>
{
  public:
    void init() override
    {
        memset(&amx, 0, sizeof(amx));
        vmMemory.fill(0);
        currentScript.clear();

        //         std::string scriptPath = "fat:/data/scripts/example_event.amx";
        //
        //         int err = aux_LoadProgram(&amx, scriptPath.c_str(), vmMemory.data());
        //         logMessage(LogSeverity::DEBUG, "aux_LoadProgram result: " + std::to_string(err));
        //
        //         if (err != AMX_ERR_NONE)
        //         {
        //             logMessage(LogSeverity::ERROR, "Pawn: Failed to load script: " + scriptPath);
        //             return;
        //         }
        //
        //         const AMX_NATIVE_INFO abc[] = {{"print", print}, {"printf", n_printf}, {0, 0}};
        //
        //         int regErr = amx_Register(&amx, abc, -1);
        //         logMessage(LogSeverity::DEBUG, "amx_Register result: " + std::to_string(regErr));

        //         cell ret = 0;
        //         int execErr = amx_Exec(&amx, &ret, AMX_EXEC_MAIN);
        //         logMessage(LogSeverity::DEBUG, "amx_Exec result: " + std::to_string(execErr) + ", ret: " + std::to_string(ret));
    }

    void process() override
    {
        if (status == VmStatus::RUNNING)
        {
        }
    }

    void shutdown() override
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
    void unloadScript();
    bool runScript();
    bool resumeScript();
    void stopScript();
    void pauseScript();

    [[nodiscard]] bool canScriptFit(const std::string& scriptPath) const;

    [[nodiscard]] bool hasPublic(std::string_view funcName) const;
    int callPublic(std::string_view funcName);
    int callPublic(std::string_view funcName, etl::span<const cell> params, cell* retVal = nullptr);

    [[nodiscard]] bool hasPublicVariable(std::string_view varName) const;
    bool getPublicVariable(std::string_view varName, cell& outValue) const;
    bool setPublicVariable(std::string_view varName, cell value);
    cell* getPublicVariableAddress(std::string_view varName);

    [[nodiscard]] VmStatus getStatus() const
    {
        return status;
    }
    [[nodiscard]] const std::string& getCurrentScript() const
    {
        return currentScript;
    }

    [[nodiscard]] bool isLoaded() const
    {
        return status == VmStatus::READY || amx.base != nullptr;
    }

    [[nodiscard]] bool isRunning() const
    {
        return status == VmStatus::RUNNING;
    }

    [[nodiscard]] bool isSleeping() const
    {
        return status == VmStatus::SLEEPING;
    }

    [[nodiscard]] bool isFinished() const
    {
        return status == VmStatus::FINISHED;
    }

    [[nodiscard]] int getLastError() const;
    [[nodiscard]] const char* getLastErrorString() const;

    [[nodiscard]] size_t getMemoryUsage() const;
    [[nodiscard]] size_t getHeapUsage() const;
    [[nodiscard]] size_t getStackRemaining() const;

    static constexpr size_t getMaxMemory()
    {
        return MAX_MEMORY_USAGE;
    }

  private:
    friend class Singleton<ScriptManager>;
    ScriptManager() = default;

    static constexpr size_t MAX_MEMORY_USAGE = 6400 * sizeof(cell);

    VmStatus status = VmStatus::UNINITIALIZED;
    int lastError;
    std::string currentScript;

    AMX amx{};
    alignas(sizeof(cell)) inline static etl::array<uint8_t, MAX_MEMORY_USAGE> vmMemory;

    IOManager& io = IOManager::getInstance();

    void setStatus();
};
