/**
 * @file ScriptingSystem.hpp
 * @brief System for loading and running scripts.
 * @author nothing
 */

#pragma once

#include "etl/array.h"
#include "etl/span.h"
#include "managers/IOManager.hpp"
#include "types/ScriptTypes.hpp"
#include <aegis/system.hpp>
#include <amx.h>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>

class ScriptingSystem : public ae::System, public ae::Singleton<ScriptingSystem>
{
  public:
    void init() override;
    void update(ae::q20_12_t /*dt*/) override;
    void shutdown() override;

    std::expected<void, ScriptError> loadScript(const std::string&);
    std::expected<void, ScriptError> reloadScript();
    void reset();
    std::expected<void, ScriptError> runScript();
    std::expected<void, ScriptError> resumeScript();
    void stopScript();
    void pauseScript();

    [[nodiscard]] bool canScriptFit(const std::string& scriptPath) const;

    [[nodiscard]] bool hasPublic(const std::string& funcName) const;
    std::expected<cell, ScriptError> callPublic(const std::string& funcName, etl::span<const cell> params = {});

    [[nodiscard]] bool hasPublicVariable(const std::string& varName) const;
    std::expected<cell, ScriptError> getPublicVariable(const std::string& varName) const;
    std::expected<void, ScriptError> setPublicVariable(const std::string& varName, cell value);
    [[nodiscard]] const cell* getPublicVariableAddress(const std::string& varName) const;
    cell* getPublicVariableAddress(const std::string& varName);

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
        return status == VmStatus::RUNNING || status == VmStatus::YIELDED;
    }

    [[nodiscard]] bool isSleeping() const
    {
        return status == VmStatus::SLEEPING;
    }

    [[nodiscard]] bool isFinished() const
    {
        return status == VmStatus::FINISHED;
    }

    [[nodiscard]] bool isResumable() const
    {
        return status == VmStatus::SLEEPING || status == VmStatus::STOPPED || status == VmStatus::YIELDED;
    }

    [[nodiscard]] ScriptError getLastError() const
    {
        return lastError;
    }

    [[nodiscard]] const char* getLastErrorString() const
    {
        return toString(lastError);
    }

    [[nodiscard]] size_t getMemoryUsage() const
    {
        if (amx.base == nullptr)
        {
            return 0;
        }
        const auto* hdr = reinterpret_cast<const AMX_HEADER*>(amx.base);

        return static_cast<size_t>(hdr->stp);
    }

    [[nodiscard]] size_t getHeapUsage() const
    {
        if (amx.base == nullptr || amx.hea < amx.hlw)
        {
            return 0;
        }

        return static_cast<size_t>(amx.hea - amx.hlw);
    }

    [[nodiscard]] size_t getStackRemaining() const
    {
        if (amx.base == nullptr || amx.stk <= amx.hea)
        {
            return 0;
        }

        return static_cast<size_t>(amx.stk - amx.hea);
    }

    static constexpr size_t getMaxMemory()
    {
        return MAX_MEMORY_USAGE;
    }

    void setActive(bool isActive)
    {
        active = isActive;
    }

  private:
    friend class Singleton<ScriptingSystem>;
    ScriptingSystem() = default;

    static constexpr size_t MAX_MEMORY_USAGE = 6400 * sizeof(cell);

    VmStatus status = VmStatus::UNINITIALIZED;
    ScriptError lastError = ScriptError::None;
    std::string currentScript;

    AMX amx{};
    alignas(cell) inline static etl::array<uint8_t, MAX_MEMORY_USAGE> vmMemory;

    IOManager& io = IOManager::getInstance();

    void setStatus(VmStatus);
    std::expected<void, ScriptError> execute(int);
};
