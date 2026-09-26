#include "controllers/AnimationController.hpp"
#include <stdio.h>
#include <string.h>

AnimationController* AnimationController::instance = nullptr;

void AnimationController::create()
{
    if (instance == nullptr)
    {
        instance = new AnimationController();
    }
}

void AnimationController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

AnimationController* AnimationController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
    unloadTextures();
}

int AnimationController::textureSizeToEnum(int px)
{
    if (px <= 8)
        return TEXTURE_SIZE_8;
    if (px <= 16)
        return TEXTURE_SIZE_16;
    if (px <= 32)
        return TEXTURE_SIZE_32;
    if (px <= 64)
        return TEXTURE_SIZE_64;
    if (px <= 128)
        return TEXTURE_SIZE_128;
    if (px <= 256)
        return TEXTURE_SIZE_256;
    if (px <= 512)
        return TEXTURE_SIZE_512;
    return TEXTURE_SIZE_1024;
}

// Identify the compiled model format.
static ModelVersion readModelVersion(FILE* file)
{
    char magic[4];

    fread(magic, 1, 4, file);

    if (strncmp(magic, "MDL1", 4) == 0)
        return ModelVersion::MDL1;

    if (strncmp(magic, "MDL2", 4) == 0)
        return ModelVersion::MDL2;

    return ModelVersion::INVALID;
}

bool AnimationController::loadModel(const char* filepath)
{
    FILE* file = fopen(filepath, "rb");
    if (!file)
        return false;

    ModelVersion version = readModelVersion(file);
    if (version == ModelVersion::INVALID)
    {
        fclose(file);
        return false;
    }

    u32 numNodes, numAnims;
    fread(&numNodes, sizeof(u32), 1, file);
    fread(&numAnims, sizeof(u32), 1, file);

    if (version == ModelVersion::MDL2)
    {
        u32 texCount;
        fread(&texCount, sizeof(u32), 1, file);
        // Texture metadata is supplied by the generated model header.
        fseek(file, (long)(texCount * 72), SEEK_CUR);
    }

    modelNodes.resize(numNodes);
    rootNodes.clear();

    for (u32 i = 0; i < numNodes; i++)
    {
        AnimNode& node = modelNodes[i];
        node.id = i;
        node.children.clear();
        node.subLists.clear();

        s32 pid, px, py, pz;
        fread(&pid, sizeof(s32), 1, file);
        fread(&px, sizeof(s32), 1, file);
        fread(&py, sizeof(s32), 1, file);
        fread(&pz, sizeof(s32), 1, file);

        node.parentId = pid;
        node.pivotX = (v16)px;
        node.pivotY = (v16)py;
        node.pivotZ = (v16)pz;

        if (version == ModelVersion::MDL2)
        {
            u32 subListCount;
            fread(&subListCount, sizeof(u32), 1, file);
            node.subLists.resize(subListCount);

            for (u32 s = 0; s < subListCount; s++)
            {
                SubList& sl = node.subLists[s];
                s32 texSlot;
                u32 dlSize;
                fread(&texSlot, sizeof(s32), 1, file);
                fread(&dlSize, sizeof(u32), 1, file);

                sl.texSlot = (int)texSlot;
                sl.displayListSize = dlSize;
                sl.displayList.resize(dlSize + 1);
                sl.displayList[0] = dlSize;
                if (dlSize > 0)
                {
                    fread(&sl.displayList[1], sizeof(u32), dlSize, file);
                }
            }
        }
        else
        {
            // Legacy models contain one untextured display list per node.
            u32 dlSize;
            fread(&dlSize, sizeof(u32), 1, file);

            SubList sl;
            sl.texSlot = -1;
            sl.displayListSize = dlSize;
            sl.displayList.resize(dlSize + 1);
            sl.displayList[0] = dlSize;
            if (dlSize > 0)
            {
                fread(&sl.displayList[1], sizeof(u32), dlSize, file);
            }
            node.subLists.push_back(sl);
        }

        if (pid == -1)
        {
            rootNodes.push_back(i);
        }
        else if (pid >= 0 && pid < (int)numNodes)
        {
            modelNodes[pid].children.push_back(i);
        }
    }

    animations.resize(numAnims);
    for (u32 i = 0; i < numAnims; i++)
    {
        Animation& anim = animations[i];
        fseek(file, 32, SEEK_CUR); // skip the 32-byte name string
        fread(&anim.duration, sizeof(u32), 1, file);

        anim.nodeTracks.resize(numNodes);
        for (u32 n = 0; n < numNodes; n++)
        {
            AnimTrack& track = anim.nodeTracks[n];
            u32 numFrames;
            fread(&numFrames, sizeof(u32), 1, file);
            track.frames.resize(numFrames);
            if (numFrames > 0)
            {
                fread(track.frames.data(), sizeof(Keyframe), numFrames, file);
            }
        }
    }

    fclose(file);
    trackIndices.assign(modelNodes.size(), 0);
    return true;
}

bool AnimationController::loadTextures(
    int count, const unsigned int** bitmaps, const int* widths, const int* heights, const bool* isRGBA)
{
    unloadTextures();

    textureIDs.resize(count, 0);

    for (int i = 0; i < count; i++)
    {
        if (!bitmaps || !bitmaps[i])
            continue;

        glGenTextures(1, &textureIDs[i]);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        GL_TEXTURE_TYPE_ENUM fmt = isRGBA[i] ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     fmt,
                     textureSizeToEnum(widths[i]),
                     textureSizeToEnum(heights[i]),
                     0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                     bitmaps[i]);
    }

    return true;
}

void AnimationController::unloadTextures()
{
    if (textureIDs.empty())
        return;

    glDeleteTextures((int)textureIDs.size(), textureIDs.data());
    textureIDs.clear();
}

void AnimationController::set(int animIndex, bool loop)
{
    if (animIndex < 0 || animIndex >= (int)animations.size())
        return;
    currentAnimIndex = animIndex;
    currentFrameFP = 0;
    animSpeedFP = 128; // 30FPS to 60FPS
    isFinishing = false;
    isLooping = loop;
    fill(trackIndices.begin(), trackIndices.end(), 0);
}

void AnimationController::play()
{
    if (currentAnimIndex == -1)
        return;
    isPlaying = true;
    isFinishing = false;
    if ((currentFrameFP >> 8) >= animations[currentAnimIndex].duration - 1)
    {
        currentFrameFP = 0;
        fill(trackIndices.begin(), trackIndices.end(), 0);
    }
}

void AnimationController::stop()
{
    isPlaying = false;
    isFinishing = false;
    currentFrameFP = 0;
    fill(trackIndices.begin(), trackIndices.end(), 0);
}

void AnimationController::pause()
{
    isFinishing = true;
}

void AnimationController::update()
{
    if (!isPlaying || currentAnimIndex == -1)
        return;

    currentFrameFP += animSpeedFP;
    int currentFrameInt = currentFrameFP >> 8;

    if (currentFrameInt >= animations[currentAnimIndex].duration)
    {
        if (isFinishing || !isLooping)
        {
            isPlaying = false;
            isFinishing = false;
            if (!isLooping)
            {
                currentFrameFP = (animations[currentAnimIndex].duration - 1) << 8;
            }
            else
            {
                currentFrameFP = 0;
                fill(trackIndices.begin(), trackIndices.end(), 0);
            }
        }
        else
        {
            currentFrameFP = 0;
            fill(trackIndices.begin(), trackIndices.end(), 0);
        }
    }
}

Keyframe AnimationController::getInterpolatedFrame(const AnimTrack& track, int timeFP, int nodeId)
{
    if (track.frames.empty())
        return {0, 0, 0, 0, 0, 0, 0};
    if (track.frames.size() == 1)
        return track.frames[0];

    int& cacheIdx = trackIndices[nodeId];

    int timeInt = timeFP >> 8;

    if (cacheIdx >= (int)track.frames.size() - 1 || timeInt < track.frames[cacheIdx].time)
    {
        cacheIdx = 0;
    }
    while (cacheIdx < (int)track.frames.size() - 1 && timeInt >= track.frames[cacheIdx + 1].time)
    {
        cacheIdx++;
    }

    Keyframe k1 = track.frames[cacheIdx];
    if (cacheIdx >= (int)track.frames.size() - 1)
        return k1;

    Keyframe k2 = track.frames[cacheIdx + 1];
    int range = k2.time - k1.time;
    if (range == 0)
        return k1;

    int progressFP = timeFP - (k1.time << 8);
    if (progressFP == 0)
        return k1;

    int t = (progressFP << 4) / range; // converts 8-bit frac to 12-bit frac (0..4096)

    Keyframe result;
    result.time = timeInt;
    result.posX = k1.posX + (((k2.posX - k1.posX) * t) >> 12);
    result.posY = k1.posY + (((k2.posY - k1.posY) * t) >> 12);
    result.posZ = k1.posZ + (((k2.posZ - k1.posZ) * t) >> 12);
    result.rotX = k1.rotX + (((k2.rotX - k1.rotX) * t) >> 12);
    result.rotY = k1.rotY + (((k2.rotY - k1.rotY) * t) >> 12);
    result.rotZ = k1.rotZ + (((k2.rotZ - k1.rotZ) * t) >> 12);
    return result;
}

void AnimationController::render()
{
    if (modelNodes.empty())
        return;
    glMatrixMode(GL_MODELVIEW);
    for (int rootId : rootNodes)
    {
        renderNode(rootId);
    }
    // Wait until the geometry FIFO has consumed all commands.
    while (GFX_BUSY)
        ;
}

void AnimationController::renderNode(int nodeId)
{
    AnimNode& node = modelNodes[nodeId];
    glPushMatrix();

    // Apply the current node transform around its pivot.
    if (currentAnimIndex != -1)
    {
        AnimTrack& track = animations[currentAnimIndex].nodeTracks[nodeId];
        Keyframe current = getInterpolatedFrame(track, currentFrameFP, nodeId);

        glTranslatef32(node.pivotX + current.posX, node.pivotY + current.posY, node.pivotZ + current.posZ);
        glRotateZi(current.rotZ);
        glRotateYi(current.rotY);
        glRotateXi(current.rotX);
        glTranslatef32(-node.pivotX, -node.pivotY, -node.pivotZ);
    }

    // Bind textures only when the sub-list changes texture slots.
    int currentTexSlot = -2; // -2 == "nothing bound yet this node"

    for (SubList& sl : node.subLists)
    {
        if (sl.displayListSize == 0)
            continue;

        if (sl.texSlot != currentTexSlot)
        {
            while (GFX_BUSY)
                ;
            if (sl.texSlot >= 0 && sl.texSlot < (int)textureIDs.size())
            {
                glBindTexture(GL_TEXTURE_2D, textureIDs[sl.texSlot]);
            }
            currentTexSlot = sl.texSlot;
        }

        glCallList(sl.displayList.data());
    }

    // Render child nodes recursively.
    for (int childId : node.children)
    {
        renderNode(childId);
    }

    glPopMatrix(1);
}
