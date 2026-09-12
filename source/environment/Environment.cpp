#include "Environment.hpp"
#include "core/globals.hpp"
#include <math.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

Environment::Environment() : dbEntry(nullptr)
{
    for (int i = 0; i < MAX_ENVIRONMENT_TEXTURES; i++)
    {
        displayLists[i] = nullptr;
        dlSizes[i] = 0;
        textureIDs[i] = 0;
    }
}

bool Environment::load(const EnvironmentDbEntry* entry,
                       std::array<const unsigned int*, MAX_ENVIRONMENT_TEXTURES> bitmaps)
{
    cleanup();

    // Guard against a missing/oversized db entry before touching it
    if (!entry || entry->textureCount > MAX_ENVIRONMENT_TEXTURES)
    {
        printf("EnvironmentDbEntry textures exceeds MAX_ENVIRONMENT_TEXTURES");
        return false;
    }

    dbEntry = entry;

    const std::string fullBinaryPath = fatBasePath + "environments/" + entry->name + "/" + entry->binaryFile;

    if (Globals::enableDebugPrint)
    {
        printf("Environment::load opening '%s'\n", fullBinaryPath.c_str());
    }

    FILE* file = fopen(fullBinaryPath.c_str(), "rb");
    if (!file)
    {
        cleanup();
        return false;
    }

    char magic[4];
    if (fread(magic, 1, 4, file) != 4)
    {
        fclose(file);
        cleanup();
        return false;
    }

    if (magic[0] != 'E' || magic[1] != 'N' || magic[2] != 'V' || magic[3] != '1')
    {
        fclose(file);
        cleanup();
        return false;
    }

    u32 groupCount = 0;
    if (fread(&groupCount, sizeof(u32), 1, file) != 1)
    {
        fclose(file);
        cleanup();
        return false;
    }

    if (groupCount > (u32)entry->textureCount)
    {
        fclose(file);
        cleanup();
        return false;
    }

    // Display list load
    for (u32 i = 0; i < groupCount; i++)
    {
        if (fread(&dlSizes[i], sizeof(u32), 1, file) != 1)
        {
            fclose(file);
            cleanup(); // frees any displayLists[0..i) already allocated this call
            return false;
        }

        displayLists[i] = nullptr;

        if (dlSizes[i] > 0)
        {
            displayLists[i] = (u32*)malloc((dlSizes[i] + 1) * sizeof(u32));

            if (!displayLists[i])
            {
                fclose(file);
                cleanup();
                return false;
            }

            displayLists[i][0] = dlSizes[i];

            if (fread(&displayLists[i][1], sizeof(u32), dlSizes[i], file) != dlSizes[i])
            {
                fclose(file);
                cleanup();
                return false;
            }
        }
    }

    fclose(file);

    // Texture upload
    for (int i = 0; i < entry->textureCount; i++)
    {
        textureIDs[i] = 0;

        if (bitmaps.empty() || !bitmaps[i])
            continue;

        render.uploadTexture(textureIDs[i],
                             GL_RGBA,
                             entry->textures[i].width,
                             entry->textures[i].height,
                             TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                             bitmaps[i]);
    }

    return true;
}

void Environment::draw()
{
    if (!dbEntry)
        return;

    for (int i = 0; i < dbEntry->textureCount; i++)
    {
        if (!textureIDs[i])
            continue;

        render.renderTexturedModel(displayLists[i], textureIDs[i]);
    }
}

void Environment::drawBillboards(bool faceCamera, ae::q20_12_t camX, ae::q20_12_t camY, ae::q20_12_t camZ)
{
    if (!dbEntry || dbEntry->billboardCount == 0)
        return;

    for (int i = 0; i < dbEntry->billboardCount; i++)
    {
        const auto& bb = dbEntry->billboards[i];
        // Skip any billboard that references an out-of-range texture slot
        if (bb.texSlot >= dbEntry->textureCount)
            continue;
        // Skip any billboard that references a texture slot that has no bound texture ID
        if (!textureIDs[bb.texSlot])
            continue;

        render.renderTexturedBillboard(bb, textureIDs[bb.texSlot], faceCamera, camX, camY, camZ);
    }
}

void Environment::cleanup()
{
    for (int i = 0; i < MAX_ENVIRONMENT_TEXTURES; i++)
    {
        if (displayLists[i])
        {
            free(displayLists[i]);
            displayLists[i] = nullptr;
        }

        dlSizes[i] = 0;

        if (textureIDs[i])
        {
            // Previously this only zeroed the id without ever releasing the
            // underlying GPU texture slot, leaking VRAM texture memory on
            // every single room transition.
            render.deleteTexture(textureIDs[i]);
        }
    }

    dbEntry = nullptr;
}
