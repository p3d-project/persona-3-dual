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
