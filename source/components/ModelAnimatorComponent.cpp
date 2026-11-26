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
    animations.resize(animCount);
    for (uint32_t i = 0; i < animCount; i++)
    {
        Animation_N& anim = animations[i];

        if (fread(&anim.name, 1, 32, f) != 32 || fread(&anim.numFrames, sizeof(int16_t), 1, f) != 1 ||
            fread(&anim.fps, sizeof(int16_t), 1, f) != 1)
        {
            return false;
        }

        uint32_t trackCount = 0;
        if (fread(&trackCount, sizeof(uint32_t), 1, f) != 1)
        {
            return false;
        }

        anim.tracks.resize(trackCount);
        for (uint32_t t = 0; t < trackCount; t++)
        {
            NodeTrack& track = anim.tracks[t];
            if (fread(&track.nodeIndex, sizeof(int32_t), 1, f) != 1)
            {
                return false;
            }

            uint32_t posKeyCount = 0;
            if (fread(&posKeyCount, sizeof(uint32_t), 1, f) != 1)
            {
                return false;
            }

            track.translations.resize(posKeyCount);
            for (uint32_t k = 0; k < posKeyCount; k++)
            {
                RawTrackTranslationData rawTrans;
                if (fread(&rawTrans, sizeof(RawTrackTranslationData), 1, f) != 1)
                {
                    return false;
                }
                track.translations[k].frame = rawTrans.frame;
                track.translations[k].x = rawTrans.x;
                track.translations[k].y = rawTrans.y;
                track.translations[k].z = rawTrans.z;
            }

            uint32_t rotKeyCount = 0;
            if (fread(&rotKeyCount, sizeof(uint32_t), 1, f) != 1)
            {
                return false;
            }

            track.rotations.resize(rotKeyCount);
            for (uint32_t k = 0; k < rotKeyCount; k++)
            {
                RawTrackRotationData rawRot;
                if (fread(&rawRot, sizeof(RawTrackRotationData), 1, f) != 1)
                {
                    return false;
                }
                track.rotations[k].frame = rawRot.frame;
                track.rotations[k].x = rawRot.x;
                track.rotations[k].y = rawRot.y;
                track.rotations[k].z = rawRot.z;
                track.rotations[k].w = rawRot.w;
            }
        }
    }
    return true;
}

void ModelAnimatorComponent::applyTransformationForNode(int nodeIndex)
{
    // Get Keys for Node at current time
    PositionKey posKey = animations[currentAnimationIndex].tracks[nodeIndex].translations[currentFrame];
    QuaternionKey rotKey = animations[currentAnimationIndex].tracks[nodeIndex].rotations[currentFrame];

    // Apply Transformation
    if (posKey.x != 0 || posKey.y != 0 || posKey.z != 0)
    {
        render.TranslateMatrix(posKey.x, posKey.y, posKey.z);
    }
    if (rotKey.x != 0 || rotKey.y != 0 || rotKey.z != 0 || rotKey.w != 4096)
    {
        render.RotateMatrix(rotKey.x, rotKey.y, rotKey.z, rotKey.w);
    }
}

const NodeTrack* ModelAnimatorComponent::findNodeTrack(const Animation_N* anim, int32_t nodeIndex) const
{
    if (!anim)
    {
        return nullptr;
    }
    if (nodeIndex >= 0 && static_cast<size_t>(nodeIndex) < anim->tracks.size())
    {
        return &anim->tracks[nodeIndex];
    }
    return nullptr;
}
