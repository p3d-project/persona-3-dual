#include "SaveSystem.hpp"
#include "core/globals.hpp"

void SaveSystem::on_receive(const Event::ReadSave)
{
    bool success = io.readFile<Save>(&saveData, "save/save.sav");
    if (!success)
    {
        consoleDemoInit();
        printf("Failed to read save data!\n");
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    // down the line, do checksum validation to ensure save file is not corrupted

    if (saveData.header.version.majorVersion != gameVersion.majorVersion ||
        saveData.header.version.minorVersion != gameVersion.minorVersion ||
        saveData.header.version.patchVersion != gameVersion.patchVersion)
    {
        bool res = migrate_save_version(&saveData);
        if (!res)
        {
            // raise some sort of error here, emit event to prompt player if they would like to erase save and create fresh one
        }
    }
}

void SaveSystem::on_receive(const Event::WriteSave)
{
    bool success = io.writeFile<Save>(&saveData, "save/save.sav");
    if (!success)
    {
        consoleDemoInit();
        printf("Failed to write save data!\n");
        while (1)
        {
            swiWaitForVBlank();
        }
    }
}

bool SaveSystem::migrate_save_version(Save* saveData)
{
    saveData->header.version.majorVersion = gameVersion.majorVersion;
    saveData->header.version.minorVersion = gameVersion.minorVersion;
    saveData->header.version.patchVersion = gameVersion.patchVersion;
    return true;
}
