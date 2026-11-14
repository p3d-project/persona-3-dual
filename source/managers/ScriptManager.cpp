#include "ScriptManager.hpp"

bool ScriptManager::canScriptFit(const std::string& scriptPath) const
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
        return true;
    }

    return false;
}
