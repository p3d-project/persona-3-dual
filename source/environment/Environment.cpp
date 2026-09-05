#include "Environment.hpp"
#include "core/globals.hpp"
#include <math.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

/**
 * @brief Converts a raw texture dimension in pixels to the corresponding
 *        libnds TEXTURE_SIZE_* enum value.
 *
 * @param size Texture width/height in pixels. Expected to be one of the
 *             power-of-two values 8, 16, 32, 64, 128, 256, 512, or 1024.
 * @return The matching TEXTURE_SIZE_* constant, or TEXTURE_SIZE_8 as a
 *         fallback if @p size does not match a supported value (a message
 *         is also printed to the debug console in that case).
 */
static int textureSizeEnum(int size)
{
    switch (size)
    {
    case 8:
        return TEXTURE_SIZE_8;
    case 16:
        return TEXTURE_SIZE_16;
    case 32:
        return TEXTURE_SIZE_32;
    case 64:
        return TEXTURE_SIZE_64;
    case 128:
        return TEXTURE_SIZE_128;
    case 256:
        return TEXTURE_SIZE_256;
    case 512:
        return TEXTURE_SIZE_512;
    case 1024:
        return TEXTURE_SIZE_1024;
    default:
        printf("Invalid texture size %d\n", size);
        return TEXTURE_SIZE_8;
    }
}

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

        glGenTextures(1, &textureIDs[i]);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     textureSizeEnum(entry->textures[i].width),
                     textureSizeEnum(entry->textures[i].height),
                     0,
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

        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        if (displayLists[i])
        {
            // Guard against corrupted DL pointers
            glCallList(displayLists[i]);
        }

        while (GFX_BUSY)
            ;
    }
}

void Environment::drawBillboards(bool faceCamera, ae::q20_12_t camX, ae::q20_12_t camY, ae::q20_12_t camZ)
{
    if (!dbEntry || dbEntry->billboardCount == 0)
        return;

    int currentSlot = -1;
    bool inQuads = false;

    for (int i = 0; i < dbEntry->billboardCount; i++)
    {
        const auto& bb = dbEntry->billboards[i];

        if (bb.texSlot >= dbEntry->textureCount)
            continue;

        if (!textureIDs[bb.texSlot])
            continue;

        if (bb.texSlot != currentSlot)
        {
            if (inQuads)
            {
                glEnd();
                inQuads = false;
            }

            while (GFX_BUSY)
                ;

            glBindTexture(GL_TEXTURE_2D, textureIDs[bb.texSlot]);
            currentSlot = bb.texSlot;
        }

        if (!inQuads)
        {
            glBegin(GL_QUADS);
            inQuads = true;
        }

        ae::q4_12_t rX{1}, rY{0}, rZ{0};
        ae::q4_12_t uX{0}, uY{1}, uZ{0};

        if (faceCamera)
        {
            //cast to 32 bit type for conversion first
            ae::q20_12_t bx = ae::q20_12_t::from_raw_value(static_cast<int32_t>(bb.x.raw_value()));
            ae::q20_12_t bz = ae::q20_12_t::from_raw_value(static_cast<int32_t>(bb.z.raw_value()));

            ae::q20_12_t dx = camX - bx;
            ae::q20_12_t dz = camZ - bz;

            // Offset to align model pivot with NDS camera origin
            ae::q20_12_t dist = math.length(dx, dz, ae::q20_12_t{0});

            if (dist > ae::q20_12_t{0.001})
            {
                dx = math.div(dx, dist);
                dz = math.div(dz, dist);
            }

            // narrow type
            rX = ae::q4_12_t::from_raw_value(static_cast<int16_t>(dz.raw_value()));
            rZ = ae::q4_12_t::from_raw_value(static_cast<int16_t>((-dx).raw_value()));
        }

        ae::q4_12_t rx = rX * bb.halfWidth;
        ae::q4_12_t ry = rY * bb.halfWidth;
        ae::q4_12_t rz = rZ * bb.halfWidth;

        ae::q4_12_t ux = uX * bb.halfHeight;
        ae::q4_12_t uy = uY * bb.halfHeight;
        ae::q4_12_t uz = uZ * bb.halfHeight;

        glTexCoord2t16(bb.u0.raw_value(), bb.v1.raw_value());
        glVertex3v16((bb.x - rx - ux).raw_value(), (bb.y - ry - uy).raw_value(), (bb.z - rz - uz).raw_value());

        glTexCoord2t16(bb.u1.raw_value(), bb.v1.raw_value());
        glVertex3v16((bb.x + rx - ux).raw_value(), (bb.y + ry - uy).raw_value(), (bb.z + rz - uz).raw_value());

        glTexCoord2t16(bb.u1.raw_value(), bb.v0.raw_value());
        glVertex3v16((bb.x + rx + ux).raw_value(), (bb.y + ry + uy).raw_value(), (bb.z + rz + uz).raw_value());

        glTexCoord2t16(bb.u0.raw_value(), bb.v0.raw_value());
        glVertex3v16((bb.x - rx + ux).raw_value(), (bb.y - ry + uy).raw_value(), (bb.z - rz + uz).raw_value());
    }

    if (inQuads)
        glEnd();
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
            glDeleteTextures(1, &textureIDs[i]);
            textureIDs[i] = 0;
        }
    }

    dbEntry = nullptr;
}
