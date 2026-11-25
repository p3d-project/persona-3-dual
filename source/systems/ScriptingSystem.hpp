/**
 * @file ScriptingSystem.hpp
 * @brief System for loading, managing, and executing Pawn scripts.
 * @author Joey Luu (Nothing)
 */

#pragma once

#include "etl/array.h"
#include "etl/span.h"
#include "etl/string_view.h"

#include "managers/IOManager.hpp"
#include "types/ScriptTypes.hpp"
#include <aegis/system.hpp>
#include <amx.h>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>

/**
 * @brief System managing the Pawn AMX virtual machine runtime.
 *
 * @details The ScriptingSystem integrates the Pawn scripting language into the Aegis engine.
 * It allocates a static memory buffer (@ref vmMemory) for bytecode execution on the Nintendo DS,
 * supports cooperative multitasking across engine frames via yielding (`AMX_ERR_YIELD`), and provides
 * bidirectional interop for calling public script functions and accessing script variables from C++.
 */
class ScriptingSystem : public ae::System, public ae::Singleton<ScriptingSystem>
{
  public:
    /**
     * @brief Initializes the scripting system.
     */
    void init() override;

    /**
     * @brief Engine per-frame update loop.
     *
     * Resumes script execution if the script is currently running or yielded.
     *
     * @param dt Fixed-point delta time passed from the aegis engine loop.
     */
    void update(ae::q20_12_t dt) override;

    /**
     * @brief Cleans up the VM and resets scripting state upon engine shutdown.
     */
    void shutdown() override;

    /**
     * @brief Loads a compiled AMX script from disk into VM memory.
     *
     * Validates the AMX header magic number, size bounds, and memory requirements
     * against @ref MAX_MEMORY_USAGE. Resets any previously loaded script, copies bytecode
     * into the VM memory buffer, and initializes the AMX instance.
     *
     * @param scriptPath Path to the compiled .amx script file.
     * @return Empty expected on success, or a @ref ScriptError on failure.
     */
    std::expected<void, ScriptError> loadScript(const std::string& scriptPath);

    /**
     * @brief Reloads the currently loaded script from its original file path.
     *
     * @return Empty expected on success, or a @ref ScriptError on failure.
     */
    std::expected<void, ScriptError> reloadScript();

    /**
     * @brief Resets the virtual machine.
     */
    void reset();

    /**
     * @brief Begins execution of the loaded script's main entry point (`AMX_EXEC_MAIN`).
     *
     * @return Empty expected on success, or a @ref ScriptError on failure.
     */
    std::expected<void, ScriptError> runScript();

    /**
     * @brief Resumes execution of a paused or yielded script.
     *
     * @return Empty expected on success, or a @ref ScriptError on failure.
     */
    std::expected<void, ScriptError> resumeScript();

    /**
     * @brief Explicitly stops script execution and sets status to @ref VmStatus::STOPPED.
     * @see ScriptingSystem::resumeScript
     */
    void stopScript();

    /**
     * @brief Pauses execution of a running script, setting status to @ref VmStatus::SLEEPING.
     */
    void pauseScript();

    /**
     * @brief Checks if a script binary on disk can fit into the static VM memory buffer.
     *
     * Inspects the file's AMX header to determine memory requirements without fully loading it.
     *
     * @param scriptPath Path to the script file.
     * @return true if the script can fit within @ref MAX_MEMORY_USAGE, false otherwise.
     */
    [[nodiscard]] bool canScriptFit(const std::string& scriptPath) const;

    /**
     * @brief Checks whether a public function exists in the currently loaded script.
     *
     * @param funcName Name of the exported public function.
     * @return true if the function exists, false otherwise.
     */
    [[nodiscard]] bool hasPublicFunction(const std::string& funcName) const;

    /**
     * @brief Runs an exported public function in the loaded script.
     *
     * @note Pushes parameters onto the AMX stack in reverse order per Pawn calling conventions
     * and executes the function.
     *
     * @param funcName Name of the exported public function.
     * @param params Span of cell arguments to pass to the function.
     * @return Return the returned value as a cell on success, or a @ref ScriptError on failure.
     */
    std::expected<cell, ScriptError> callPublic(const std::string& funcName, etl::span<const cell> params = {});

    /**
    * @brief Checks whether a public variable exists in the currently loaded script.
    *
    * @param varName Name of the exported public variable.
    * @return true if the variable exists, false otherwise.
    */
    [[nodiscard]] bool hasPublicVariable(const std::string& varName) const;

    /**
     * @brief Retrieves the value of a public script variable.
     *
     * @param varName Name of the public variable.
     * @return Cell value of the variable, or a @ref ScriptError on failure.
     */
    std::expected<cell, ScriptError> getPublicVariable(const std::string& varName) const;

    /**
     * @brief Sets the value of a public script variable.
     *
     * @param varName Name of the public variable.
     * @param value Value to write to the variable cell.
     * @return Empty expected on success, or a @ref ScriptError on failure.
     */
    std::expected<void, ScriptError> setPublicVariable(const std::string& varName, cell value);

    /**
    * @brief Retrieves a const pointer to a public variable's cell in VM memory.
    * @warning You should probably use getPublicVariable() or setPublicVariable() unless you know what you are doing.
    *
    * @param varName Name of the public variable.
    * @return Pointer to the variable's cell, or nullptr if not found or not loaded.
    * @see getPublicVariable(), setPublicVariable()
    * @see getPublicVariableAddress(const std::string&)
    */
    [[nodiscard]] const cell* getPublicVariableAddress(const std::string& varName) const;

    /**
    * @brief Retrieves a mutable pointer to a public variable's cell in VM memory.
    * @warning You should probably use getPublicVariable() or setPublicVariable() unless you know what you are doing.
    *
    * @param varName Name of the public variable.
    * @return Pointer to the variable's cell, or nullptr if not found or not loaded.
    * @see getPublicVariable(), setPublicVariable()
    * @see getPublicVariableAddress(const std::string&) const
    */
    cell* getPublicVariableAddress(const std::string& varName);

    /**
     * @brief Gets the current execution status of the virtual machine.
     *
     * @return Current @ref VmStatus.
     */
    [[nodiscard]] VmStatus getStatus() const
    {
        return status;
    }

    /**
     * @brief Gets the filesystem path of the currently loaded script.
     *
     * @return Reference to the script path string.
     */
    [[nodiscard]] const std::string& getCurrentScript() const
    {
        return currentScript;
    }

    /**
     * @brief Checks if a script is currently loaded and initialized.
     *
     * @return true if loaded and ready, false otherwise.
     */
    [[nodiscard]] bool isLoaded() const
    {
        return status == VmStatus::READY || amx.base != nullptr;
    }

    /**
     * @brief Checks if the script is currently active (running or yielded).
     *
     * @return true if status is RUNNING or YIELDED, false otherwise.
     */
    [[nodiscard]] bool isRunning() const
    {
        return status == VmStatus::RUNNING || status == VmStatus::YIELDED;
    }

    /**
     * @brief Checks if the script is in a sleeping state.
     *
     * @return true if status is SLEEPING, false otherwise.
     */
    [[nodiscard]] bool isSleeping() const
    {
        return status == VmStatus::SLEEPING;
    }

    /**
     * @brief Checks if the script has completed execution.
     *
     * @return true if status is FINISHED, false otherwise.
     */
    [[nodiscard]] bool isFinished() const
    {
        return status == VmStatus::FINISHED;
    }

    /**
     * @brief Checks if the script can be resumed from its current state.
     *
     * @return true if status is SLEEPING, STOPPED, or YIELDED; false otherwise.
     */
    [[nodiscard]] bool isResumable() const
    {
        return status == VmStatus::SLEEPING || status == VmStatus::STOPPED || status == VmStatus::YIELDED;
    }

    /**
     * @brief Gets the most recent error recorded by the scripting subsystem.
     *
     * @return Last @ref ScriptError encountered.
     */
    [[nodiscard]] ScriptError getLastError() const
    {
        return lastError;
    }

    /**
     * @brief Gets a human-readable description of the last recorded error.
     *
     * @return String view describing the error.
     */
    [[nodiscard]] etl::string_view getLastErrorString() const
    {
        return toString(lastError);
    }

    /**
     * @brief Gets the total memory footprint configured by the loaded AMX binary.
     *
     * @return Total memory requirement in bytes (`hdr->stp`), or 0 if not loaded.
     */
    [[nodiscard]] size_t getMemoryUsage() const
    {
        if (amx.base == nullptr)
        {
            return 0;
        }
        const auto* hdr = reinterpret_cast<const AMX_HEADER*>(amx.base);

        return static_cast<size_t>(hdr->stp);
    }

    /**
     * @brief Gets the current dynamic heap usage within the VM.
     *
     * @return Heap allocation in bytes (`hea - hlw`), or 0 if not loaded.
     */
    [[nodiscard]] size_t getHeapUsage() const
    {
        if (amx.base == nullptr || amx.hea < amx.hlw)
        {
            return 0;
        }

        return static_cast<size_t>(amx.hea - amx.hlw);
    }

    /**
     * @brief Gets the remaining stack headroom before colliding with the heap.
     *
     * @return Stack headroom in bytes (`stk - hea`), or 0 if not loaded.
     */
    [[nodiscard]] size_t getStackRemaining() const
    {
        if (amx.base == nullptr || amx.stk <= amx.hea)
        {
            return 0;
        }

        return static_cast<size_t>(amx.stk - amx.hea);
    }

    /**
     * @brief Gets the maximum compile-time memory buffer allocated for the VM.
     *
     * @return Maximum memory size in bytes.
     */
    static constexpr size_t getMaxMemory()
    {
        return MAX_MEMORY_USAGE;
    }

    /**
     * @brief Controls whether this system is enabled.
     * @see aegis::System::active
     */
    void setActive(bool isActive)
    {
        active = isActive;
    }

  private:
    friend class Singleton<ScriptingSystem>;
    ScriptingSystem() = default;

    /// Maximum memory limit in bytes allocated for VM execution in cells.
    static constexpr size_t MAX_MEMORY_USAGE = 6400 * sizeof(cell);

    VmStatus status = VmStatus::UNINITIALIZED; ///< Current VM execution status.
    ScriptError lastError = ScriptError::None; ///< Last error recorded by the scripting system.
    std::string currentScript;                 ///< Filesystem path to the currently loaded script.

    AMX amx{}; ///< Underlying Pawn AMX virtual machine instance.
    alignas(cell) inline static etl::array<uint8_t, MAX_MEMORY_USAGE> vmMemory; ///< Static memory buffer for the VM.

    IOManager& io = IOManager::getInstance(); ///< Reference to engine I/O manager.

    /**
     * @brief Sets the current VM Status along with active state based on that status
     *
     * @param The new VM status
     */
    void setStatus(VmStatus newStatus);

    /**
     * @brief Executes AMX bytecode from the specified entry point index.
     *
     * @param execIndex AMX execution index (e.g. AMX_EXEC_MAIN, AMX_EXEC_CONT, or function index).
     * @return Empty expected on success, or a @ref ScriptError on failure.
     */
    std::expected<void, ScriptError> execute(int execIndex);
};
