#pragma once

#include <nds.h>
#include <vector>

enum class ModelVersion
{
    INVALID,
    MDL1,
    MDL2
};

enum class CharacterAnimOption
{
    TOGGLE_AUTO_ANIM = 0,
    ANIM_1 = 1,
    ANIM_2 = 2,
    ANIM_3 = 3,
    ANIM_4 = 4,
    ANIM_5 = 5,
    ANIM_6 = 6,
    ANIM_7 = 7,
    ANIM_8 = 8,
    ANIM_9 = 9,
    ANIM_10 = 10,
    ANIM_11 = 11,
    ANIM_12 = 12,
    ANIM_13 = 13,
    ANIM_14 = 14,
    ANIM_15 = 15,
    ANIM_16 = 16,
    ANIM_17 = 17,
    ANIM_18 = 18,
    ANIM_19 = 19,
    ANIM_20 = 20,
    ANIM_21 = 21,
    ANIM_22 = 22,
    ANIM_23 = 23,
    ANIM_24 = 24
};

struct Keyframe
{
    int time;
    s16 rotX, rotY, rotZ;
    s16 posX, posY, posZ;
};

struct SubList
{
    int texSlot;
    std::vector<u32> displayList;
    u32 displayListSize;
};

struct AnimTrack
{
    std::vector<Keyframe> frames;
};

struct Animation
{
    int duration;
    std::vector<AnimTrack> nodeTracks;
};

struct AnimNode
{
    int id;
    int parentId;
    std::vector<SubList> subLists;
    std::vector<int> children;
    v16 pivotX, pivotY, pivotZ;
};
