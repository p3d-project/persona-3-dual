#pragma once

#include <string>

enum class SpriteType
{
    NONE = 0,
    MOON,
    DAY_OF_WEEK,
    TIME,
    SKILL_SPRITE,
    DIALOGUE,
    BUST,
    CUSTOM,
};

struct SpriteDBEntry
{
    SpriteType type;
    int id;
    const char* filename;
};

enum class MoonSprite
{
    MOON_0 = 0,
    MOON_1,
    MOON_2,
    MOON_3,
    MOON_4,
    MOON_5,
    MOON_6,
    MOON_7,
    MOON_8,
};

enum class TimeSprite
{
    AFTER_SCHOOL_0_0 = 0,
    AFTER_SCHOOL_1_0,
    AFTERNOON_0_0,
    AFTERNOON_1_0,
    DAYTIME_0_0,
    DAYTIME_1_0,
    EARLY_MORNING_0_0,
    EARLY_MORNING_1_0,
    LATE_NIGHT_0_0,
    LATE_NIGHT_1_0,
    // TODO: make new lunchtime sprite
    LUNCHTIME_0_0,
    LUNCHTIME_1_0,
    LUNCHTIME_2_0,
    MORNING_0_0,
    MORNING_1_0,
};

enum class SkillSprite
{
    SKILLS_LEVEL = 0
};

enum class DialogueSprite
{
    BLUE_BLOCK = 0,
    WHITE_BLOCK,
    CORNER,
    EDGE,
    CORNER_GREEN,
    EDGE_GREEN
};

enum class BustSprite
{
    TOP_LEFT = 0,
    TOP_RIGHT,
    MIDDLE_LEFT,
    MIDDLE_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    // DEBUG
    EYES_NEUTRAL,
    MOUTH_NEUTRAL,
    HAPPY
};

std::string getSpriteFilename(SpriteType type, int id);
