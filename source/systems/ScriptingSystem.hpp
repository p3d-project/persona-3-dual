/**
 * @file ScriptingSystem.hpp
 * @brief System for loading and running scripts.
 * @author nothing
 */

#pragma once

#include "etl/array.h"
#include "etl/span.h"
#include "managers/IOManager.hpp"
#include <aegis/system.hpp>
#include <amx.h>
#include <amxaux.h>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>

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

class ScriptingSystem : public ae::System, public ae::Singleton<ScriptingSystem>
{
  public:
    void init() override;
    void update(ae::q20_12_t /*dt*/) override;
    void shutdown() override;

    // TODO: Should we define our own error types for better type safety?
    std::expected<void, uint32_t> loadScript(const std::string&);
    std::expected<void, uint32_t> reloadScript();
    void reset();
    std::expected<void, uint32_t> runScript();
    std::expected<void, uint32_t> resumeScript();
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

    [[nodiscard]] int getLastError() const;
    [[nodiscard]] const char* getLastErrorString() const;

    [[nodiscard]] size_t getMemoryUsage() const;
    [[nodiscard]] size_t getHeapUsage() const;
    [[nodiscard]] size_t getStackRemaining() const;

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
    uint32_t lastError = AMX_ERR_NONE;
    std::string currentScript;

    AMX amx{};
    alignas(cell) inline static etl::array<uint8_t, MAX_MEMORY_USAGE> vmMemory;

    IOManager& io = IOManager::getInstance();

    void setStatus(VmStatus);
    std::expected<void, uint32_t> execute(int);
};
