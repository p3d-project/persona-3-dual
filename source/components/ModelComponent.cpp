#include "ModelComponent.hpp"
#include <memory>

void ModelComponent::Update(ae::q20_12_t)
{
    drawMesh();
}

void ModelComponent::Destroy()
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

        model.reset();
    }
}

bool ModelComponent::loadTextureHeader(FILE* f, MDL3Texture& tex)
{
    RawTextureHeader raw;

    if (fread(&raw, sizeof(RawTextureHeader), 1, f) != 1)
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

bool ModelComponent::loadTexture(FILE* f, MDL3Texture& tex)
{
    char imgName[32];
    uint32_t byteLength = 0;

    if (fread(imgName, 1, 32, f) != 32 || fread(&byteLength, sizeof(uint32_t), 1, f) != 1)
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

    if (fread(alignedBuffer, 1, byteLength, f) != static_cast<size_t>(byteLength))
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

bool ModelComponent::loadMesh(std::string* meshFilePath)
{
    if (meshFilePath == nullptr)
    {
        return false;
    }

    FILE* f = fopen(meshFilePath->c_str(), "rb");
    if (!f)
    {
        return false;
    }

    model = std::make_unique<MDL3Model>();
    RawModelHeader rawHeader;

    if (fread(&rawHeader, sizeof(RawModelHeader), 1, f) != 1)
    {
        model.reset();
        fclose(f);
        return false;
    }

    if (memcmp(rawHeader.magic, "MDL3", 4) != 0)
    {
        fclose(f);
        model.reset();
        return false;
    }

    model->nodeCount = rawHeader.nodeCount;
    model->texCount = rawHeader.texCount;

    if (rawHeader.animCount > 0)
    {
        fclose(f);
        model.reset();
        return false; // Animations not supported for static meshes
    }

    // Read Texture Headers
    model->textures.resize(model->texCount);
    for (uint32_t i = 0; i < model->texCount; ++i)
    {
        if (!loadTextureHeader(f, model->textures[i]))
        {
            fclose(f);
            model.reset();
            return false;
        }
    }

    // Read in Nodes
    model->nodes.resize(model->nodeCount);
    for (uint32_t i = 0; i < model->nodeCount; ++i)
    {
        RawNodeHeader rawNode;

        if (fread(&rawNode, sizeof(RawNodeHeader), 1, f) != 1)
        {
            fclose(f);
            model.reset();
            return false;
        }

        model->nodes[i].pid = rawNode.pid;

        for (uint32_t j = 0; j < rawNode.subListCount; ++j)
        {
            SubList_N sl;
            if (fread(&sl.texSlot, sizeof(int32_t), 1, f) != 1 || fread(&sl.dlSize, sizeof(uint32_t), 1, f) != 1)
            {
                fclose(f);
                model.reset();
                return false;
            }

            if (sl.dlSize > 0)
            {
                // Allocate persistent memory for the display list
                uint32_t* displayList = new uint32_t[sl.dlSize + 1];
                displayList[0] = sl.dlSize;
                if (fread(&displayList[1], sizeof(std::uint32_t), sl.dlSize, f) != static_cast<size_t>(sl.dlSize))
                {
                    std::fclose(f);
                    model.reset();
                    return false;
                }
                sl.displayList = displayList;
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
        if (!loadTexture(f, model->textures[i]))
        {
            fclose(f);
            model.reset();
            return false;
        }
    }
    fclose(f);
    isActive = true;
    return true;
}

void ModelComponent::drawMesh()
{
    render.renderModelComponent(*model);
}
