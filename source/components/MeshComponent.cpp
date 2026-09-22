#include "MeshComponent.hpp"
#include <memory>

void MeshComponent::Update(ae::q20_12_t)
{
    drawMesh();
}

void MeshComponent::Destroy()
{
    isActive = false;

    if (model)
    {
        for (MDL3Texture& tex : model->textures)
        {
            if (tex.textureID != -1)
            {
                render.deleteTexture(tex.textureID);
            }
        }

        for (Node& node : model->nodes)
        {
            for (SubList_N& sl : node.subLists)
            {
                if (sl.displayList)
                {
                    delete[] sl.displayList;
                    sl.displayList = nullptr;
                }
            }
        }

        for (Animation_N* anim : model->animations)
        {
            delete anim;
        }

        model.reset();
    }
}

bool MeshComponent::loadTextureHeader(FileBuffer& buffer, MDL3Texture& tex, size_t& offset)
{
    RawTextureHeader raw;

    if (buffer.read(&raw, sizeof(RawTextureHeader), 1, &offset) != 1)
    {
        return false;
    }

    tex.width = raw.width;
    tex.height = raw.height;
    tex.isRGBA = raw.isRGBA;

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

bool MeshComponent::loadTexture(FileBuffer& buffer, MDL3Texture& tex, size_t& offset)
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

    // Explicitly delete old VRAM textures if previously allocated
    if (tex.textureID != -1)
    {
        render.deleteTexture(tex.textureID);
    }

    GL_TEXTURE_TYPE_ENUM texType = tex.isRGBA ? GL_RGBA : GL_RGB16;
    if (!render.uploadTexture(tex.textureID,
                              texType,
                              tex.width,
                              tex.height,
                              TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                              alignedBuffer))
    {
        free(alignedBuffer);
        return false;
    }
    free(alignedBuffer);
    return true;
}

bool MeshComponent::loadMesh(std::string* meshFilePath)
{
    if (meshFilePath == nullptr)
    {
        return false;
    }

    model = std::make_unique<MDL3Model>();

    FileBuffer buffer = io.openFileBuffer(*meshFilePath);
    if (buffer.get() == nullptr)
    {
        model.reset();
        return false;
    }

    size_t offset = 0;
    RawModelHeader rawHeader;

    if (buffer.read(&rawHeader, sizeof(RawModelHeader), 1, &offset) != 1)
    {
        model.reset();
        return false;
    }

    if (memcmp(rawHeader.magic, "MDL3", 4) != 0)
    {
        model.reset();
        return false;
    }

    model->nodeCount = rawHeader.nodeCount;
    model->texCount = rawHeader.texCount;

    if (rawHeader.animCount > 0)
    {
        model.reset();
        return false; // Animations not supported for static meshes
    }

    // Read Texture Headers
    model->textures.resize(model->texCount);
    for (uint32_t i = 0; i < model->texCount; ++i)
    {
        if (!loadTextureHeader(buffer, model->textures[i], offset))
        {
            model.reset();
            return false;
        }
    }

    // Read in Nodes
    model->nodes.resize(model->nodeCount);
    for (uint32_t i = 0; i < model->nodeCount; ++i)
    {
        RawNodeHeader rawNode;

        if (buffer.read(&rawNode, sizeof(RawNodeHeader), 1, &offset) != 1)
        {
            model.reset();
            return false;
        }

        model->nodes[i].pid = rawNode.pid;
        model->nodes[i].px = rawNode.posX;
        model->nodes[i].py = rawNode.posY;
        model->nodes[i].pz = rawNode.posZ;

        for (uint32_t j = 0; j < rawNode.subListCount; ++j)
        {
            SubList_N sl;
            if (buffer.read(&sl.texSlot, sizeof(int32_t), 1, &offset) != 1 ||
                buffer.read(&sl.dlSize, sizeof(uint32_t), 1, &offset) != 1)
            {
                model.reset();
                return false;
            }

            if (sl.dlSize > 0)
            {
                const size_t wordCount = sl.dlSize;

                // Out of file bounds check
                if (offset > buffer.length() || wordCount > (buffer.length() - offset) / sizeof(uint32_t))
                {
                    model.reset();
                    return false;
                }

                const size_t rawByteSize = wordCount * sizeof(uint32_t);

                // Allocate persistent memory for the display list
                uint32_t* displayList = new uint32_t[wordCount + 1];
                displayList[0] = wordCount;
                memcpy(&displayList[1], static_cast<const uint8_t*>(buffer.get()) + offset, rawByteSize);
                sl.displayList = displayList;
                offset += rawByteSize;
            }
            model->nodes[i].subLists.push_back(std::move(sl));
        }
    }

    for (Node& node : model->nodes)
    {
        std::sort(node.subLists.begin(),
                  node.subLists.end(),
                  [](const SubList_N& a, const SubList_N& b) { return a.texSlot < b.texSlot; });
    }

    // Skip animation data on static meshes

    // Textures
    for (uint32_t i = 0; i < model->texCount; ++i)
    {
        if (!loadTexture(buffer, model->textures[i], offset))
        {
            model.reset();
            return false;
        }
    }

    isActive = true;
    return true;
}

void MeshComponent::drawMesh()
{
    render.renderMeshComponent(*model);
}
