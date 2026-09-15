#include "MeshComponent.hpp"

void MeshComponent::Update(ae::q20_12_t)
{
}

void MeshComponent::Destroy()
{
    isActive = false;
}

bool MeshComponent::loadTextureHeader(FileBuffer& buffer, MDL3Texture& tex, size_t& offset)
{
    RawTextureHeader raw;

    //if (fread(rawName, 1, 64, f) != 64 || fread(&tex.width, sizeof(uint16_t), 1, f) != 1 ||
    //    fread(&tex.height, sizeof(uint16_t), 1, f) != 1 || fread(&isRGBA, sizeof(uint8_t), 1, f) != 1 ||
    //    fread(pad, sizeof(uint8_t), 3, f) != 3)
    if (buffer.read(&raw, sizeof(RawTextureHeader), 1, &offset) != 1)
    {
        return false;
    }

    tex.width = raw.width;
    tex.height = raw.height;
    tex.isRGBA = (raw.isRGBA != 0);

    memcpy(tex.name, raw.name, 64);
    tex.name[64] = '\0'; // Ensure null-termination

    // Trim trailing spaces and nulls
    size_t len = strlen(tex.name);
    while (len > 0 && (tex.name[len - 1] == '\0' || tex.name[len - 1] == ' ' || tex.name[len - 1] == '\r'))
    {
        tex.name[--len] = '\0';
    }

    return true;
}

bool MeshComponent::loadEmbeddedImage(FileBuffer& buffer, MDL3Texture& tex, size_t& offset)
{
    char imgName[32];
    uint32_t byteLength = 0;

    if (buffer.read(imgName, 1, 32, &offset) != 32 || buffer.read(&byteLength, sizeof(uint32_t), 1, &offset) != 1)
    {
        return false;
    }
    if (byteLength == 0)
    {
        return false;
    }

    // Allocate 32-bit aligned buffer for DS DMA GPU uploads
    size_t alignedWords = (static_cast<size_t>(byteLength) + 3) / 4;
    uint32_t* alignedBuffer = static_cast<uint32_t*>(malloc(alignedWords * 4));
    if (!alignedBuffer)
    {
        return false;
    }

    if (buffer.read(alignedBuffer, 1, byteLength, &offset) != static_cast<size_t>(byteLength))
    {
        free(alignedBuffer);
        return false;
    }

    if (tex.textureID != -1)
    {
        glDeleteTextures(1, &tex.textureID); // shouldnt be here
        tex.textureID = -1;
    }

    GL_TEXTURE_TYPE_ENUM texType = tex.isRGBA ? GL_RGBA : GL_RGB16;
    render.uploadTexture(tex.textureID,
                         texType,
                         tex.width,
                         tex.height,
                         TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                         alignedBuffer);
    free(alignedBuffer);
    return true;
}

bool MeshComponent::loadMesh(std::string* meshFilePath)
{
    FileBuffer buffer = io.openFileBuffer(meshFilePath);
    if (buffer.get() == nullptr)
    {
        return false;
    }

    size_t offset = 0;

    RawModelHeader rawHeader;

    if (buffer.read(&rawHeader, sizeof(RawModelHeader), 1, &offset) != 1)
    {
        return false;
    }

    if (memcmp(rawHeader.magic, "MDL3", 4) != 0)
    {
        return false;
    }

    nodeCount = rawHeader.nodeCount;
    texCount = rawHeader.texCount;

    // Read Texture Headers
    textures.resize(texCount);
    for (uint32_t i = 0; i < texCount; ++i)
    {
        if (!loadTextureHeader(buffer, textures[i], offset))
        {
            return false;
        }
    }

    // Read in Nodes
    nodes.resize(nodeCount);
    for (uint32_t i = 0; i < nodeCount; ++i)
    {
        auto& node = nodes[i];
        uint32_t subListCount = 0;

        RawNodeHeader rawNode;

        if (buffer.read(&rawNode, sizeof(int32_t), 1, &offset) != 1)
        {
            return false;
        }

        node.pid = rawNode.pid;
        node.px = rawNode.posX;
        node.py = rawNode.posY;
        node.pz = rawNode.posZ;
        node.subLists.resize(rawNode.subListCount);
        for (uint32_t j = 0; j < rawNode.subListCount; ++j)
        {
            auto& sl = node.subLists[j];
            if (buffer.read(&sl.texSlot, sizeof(int32_t), 1, &offset) != 1 ||
                buffer.read(&sl.dlSize, sizeof(uint32_t), 1, &offset) != 1)
            {
                return false;
            }

            if (sl.dlSize > 0)
            {
                sl.displayList.resize(sl.dlSize + 1);
                sl.displayList[0] = sl.dlSize;
                if (buffer.read(&sl.displayList[1], sizeof(uint32_t), sl.dlSize, &offset) != sl.dlSize)
                {
                    return false;
                }
            }
        }
    }

    // Skip animation data for now

    // Read embedded images
    for (uint32_t i = 0; i < texCount; ++i)
    {
        if (!loadEmbeddedImage(buffer, textures[i], offset))
        {
            return false;
        }
    }

    buffer.release(); // Release the buffer after loading is complete
    return true;
}
