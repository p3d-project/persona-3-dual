#include "SaveSystem.hpp"
#include "core/globals.hpp"
#include "core/logger.hpp"

void SaveSystem::on_receive(const Event::ReadSave)
{
    bool success = io.readFile<Save>(&saveData, "save/save.sav");
    if (!success)
    {
        logMessage(LogSeverity::FATAL, "Failed to read save data!");
    }
    logMessage(LogSeverity::DEBUG, "Save data loaded successfully");

    // down the line, do checksum validation to ensure save file is not corrupted

    if (saveData.header.version != GAME_VERSION)
    {
        bool res = migrateSaveVersion(&saveData);
        if (!res)
        {
            logMessage(LogSeverity::ERROR, "Failed to migrate save file version");
            // raise some sort of error here, emit event to prompt player if they would like to erase save and create fresh one
        }
        else
        {
            logMessage(LogSeverity::DEBUG, "Save version migrated to current game version");
        }
    }
}

void SaveSystem::on_receive(const Event::WriteSave)
{
    bool success = io.writeFile<Save>(&saveData, "save/save.sav");
    if (!success)
    {
        logMessage(LogSeverity::FATAL, "Failed to write save data!");
    }
    logMessage(LogSeverity::DEBUG, "Save data written successfully");
}

bool SaveSystem::migrateSaveVersion(Save* saveData)
{
    if (saveData == nullptr)
    {
        logMessage(LogSeverity::ERROR, "Called with null saveData");
        return false;
    }
    saveData->header.version.majorVersion = GAME_VERSION.majorVersion;
    saveData->header.version.minorVersion = GAME_VERSION.minorVersion;
    saveData->header.version.patchVersion = GAME_VERSION.patchVersion;
    return true;
}
