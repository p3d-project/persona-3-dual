#include "ModelAnimatorComponent.hpp"

void ModelAnimatorComponent::Update(ae::q20_12_t /*dt*/)
{
    currentFrame++;
    // Wrap around the current frame if it exceeds the number of frames in the current animation
    if (currentFrame >= animations[currentAnimationIndex].numFrames)
    {
        currentFrame = 0;
    }
}

bool ModelAnimatorComponent::loadAnimations(FILE* f, uint32_t animCount)
{
    for (uint32_t i = 0; i < animCount; i++)
    {
        Animation_N anim = animations[i];

        if (fread(&anim.name, 1, 32, f) != 32 || fread(&anim.numFrames, sizeof(uint32_t), 1, f) != 1 ||
            fread(&anim.fps, sizeof(int16_t), 1, f) != 1)
        {
            fclose(f);
            return false;
        }

        uint32_t trackCount = 0;
        if (fread(&trackCount, sizeof(uint32_t), 1, f) != 1)
        {
            fclose(f);
            return false;
        }

        anim.tracks.resize(trackCount);
        for (uint32_t t = 0; t < trackCount; t++)
        {
            NodeTrack track = anim.tracks[t];
            if (fread(&track.nodeIndex, sizeof(int32_t), 1, f) != 1)
            {
                fclose(f);
                return false;
            }

            uint32_t posKeyCount = 0;
            if (fread(&posKeyCount, sizeof(uint32_t), 1, f) != 1)
            {
                fclose(f);
                return false;
            }

            track.translations.resize(posKeyCount);
            for (uint32_t k = 0; k < posKeyCount; k++)
            {
                // TODO: fread error handling? considering this is roundabout where the old version crashes
                fread(&track.translations[k].frame, sizeof(int16_t), 1, f);
                fread(&track.translations[k].x, sizeof(int32_t), 1, f);
                fread(&track.translations[k].y, sizeof(int32_t), 1, f);
                fread(&track.translations[k].z, sizeof(int32_t), 1, f);
            }

            uint32_t rotKeyCount = 0;
            if (fread(&rotKeyCount, sizeof(uint32_t), 1, f) != 1)
            {
                fclose(f);
                return false;
            }

            track.rotations.resize(rotKeyCount);
            for (uint32_t k = 0; k < rotKeyCount; k++)
            {
                // TODO: fread error handling? considering this is roundabout where the old version crashes
                fread(&track.rotations[k].frame, sizeof(int16_t), 1, f);
                fread(&track.rotations[k].x, sizeof(int16_t), 1, f);
                fread(&track.rotations[k].y, sizeof(int16_t), 1, f);
                fread(&track.rotations[k].z, sizeof(int16_t), 1, f);
                fread(&track.rotations[k].w, sizeof(int16_t), 1, f);
            }
        }
    }
    return true;
}

const NodeTrack* ModelAnimatorComponent::findNodeTrack(const Animation_N* anim, int32_t nodeIndex) const
{
    if (!anim)
        return nullptr;
    if (nodeIndex >= 0 && static_cast<size_t>(nodeIndex) < anim->tracks.size())
    {
        return &anim->tracks[nodeIndex];
    }
    return nullptr;
}
