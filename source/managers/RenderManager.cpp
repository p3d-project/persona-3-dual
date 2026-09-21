#include "RenderManager.hpp"
#include <nds.h>

static constexpr int32_t ONE_Q12 = 1 << 12;

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

void RenderManager::initialize3DView(View3DConfig config)
{
    glInit();

    glEnable(config.settings);
    glClearColor(config.clearColor.red, config.clearColor.green, config.clearColor.blue, config.clearColor.alpha);
    glClearPolyID(config.clearPolyID);
    glClearDepth(config.clearDepth);

    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspectivef32((int)(config.fov * DEGREE_MODIFIER),
                      config.aspect.raw_value(),
                      config.nearPlane.raw_value(),
                      floattof32(config.farPlane));

    if (config.outlineColor != -1)
    {
        glSetOutlineColor(0, config.outlineColor);
    }

    if (config.fogColor.red != -1)
    {
        glFogColor(config.fogColor.red, config.fogColor.green, config.fogColor.blue, config.fogColor.alpha);

        // How much depth difference there is between table entries
        glFogShift(config.shift);
        // Depth at which the fog starts
        glFogOffset(config.depth);

        // generate linear density table
        uint8_t density = 0;
        for (uint8_t i = 0; i < 32; ++i) // it has 32 steps
        {
            glFogDensity(i, density);
            // exponentially increase mass the furthur back the fog is
            density += (config.mass * i) >> 2;

            // entries are 7 bit, so cap the density to 127
            if (density > 127)
            {
                density = 127;
            }
        }
    }

    glPolyFmt(config.polyParams);
}

void RenderManager::cleanup3DView()
{
    activeTexture = -1;
    glClearColor(0, 0, 0, 31);
    glClearDepth(0x7FFF);
    glFlush(0);
    swiWaitForVBlank();
}

bool RenderManager::uploadTexture(int& textureID,
                                  const GL_TEXTURE_TYPE_ENUM texType,
                                  const int sizeX,
                                  const int sizeY,
                                  int param,
                                  const void* texture)
{
    if (textureID != -1 && textureID != 0)
    {
        glDeleteTextures(1, &textureID);
        textureID = -1;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // glTexImage2D can returns 0 on failure, we can use this to catch out of VRAM errors, etc.
    return glTexImage2D(GL_TEXTURE_2D, 0, texType, textureSizeEnum(sizeX), textureSizeEnum(sizeY), 0, param, texture);
}

void RenderManager::renderMeshComponent(MDL3Model& model)
{
    for (const Node& node : model.nodes)
    {
        for (const SubList_N& sl : node.subLists)
        {
            renderDisplayList(sl.displayList, model.textures[sl.texSlot].textureID);
        }
    }
}

void RenderManager::renderMeshComponent(MDL3Model& model,
                                        ae::q20_12_t posX,
                                        ae::q20_12_t posY,
                                        ae::q20_12_t posZ,
                                        uint32_t rotX,
                                        uint32_t rotY,
                                        uint32_t rotZ,
                                        ae::q20_12_t scale)
{
    glPushMatrix();
    if (posX.raw_value() || posY.raw_value() || posZ.raw_value())
    {
        glTranslatef32(posX.raw_value(), posY.raw_value(), posZ.raw_value());
    }
    if (rotX)
    {
        glRotatef32(rotX, 1, 0, 0);
    }
    if (rotY)
    {
        glRotatef32(rotY, 0, 1, 0);
    }
    if (rotZ)
    {
        glRotatef32(rotZ, 0, 0, 1);
    }
    if (scale.raw_value() != ONE_Q12)
    {
        glScalef32(scale.raw_value(), scale.raw_value(), scale.raw_value());
    }

    for (const Node& node : model.nodes)
    {
        for (const SubList_N& sl : node.subLists)
        {
            renderDisplayList(sl.displayList, model.textures[sl.texSlot].textureID);
        }
    }

    glPopMatrix(1);
}

void RenderManager::renderTexturedBillboard(
    BillboardData bb, const int texture, bool faceCamera, ae::q20_12_t camX, ae::q20_12_t camY, ae::q20_12_t camZ)
{
    if (texture != -1 && texture != activeTexture)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        activeTexture = texture;
    }
    glBegin(GL_QUADS);

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

    glEnd();
}

void RenderManager::renderDisplayList(const void* displayList, const int texture)
{
    if (!displayList)
    {
        return;
    }

    if (texture != -1 && texture != activeTexture)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        activeTexture = texture;
    }

    glCallList(displayList);
}

void RenderManager::deleteTexture(int& texture)
{
    if (texture != -1)
    {
        glDeleteTextures(1, &texture);
        texture = -1;
    }
}
