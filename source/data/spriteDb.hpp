#pragma once

#include <string>

enum class SpriteType
{
    NONE = 0,
    MOON,
    DAY_OF_WEEK,
    DIGIT,
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
    MOON_9,
    MOON_10,
    MOON_11,
    MOON_12,
    MOON_13,
    MOON_14,
    MOON_15,
    MOON_16,
    MOON_17,
    MOON_18,
    MOON_19,
    MOON_20,
    MOON_21,
    MOON_22,
    MOON_23,
    MOON_24,
    MOON_25,
    MOON_26,
    MOON_27,
    MOON_28,
    MOON_29
};

enum class DayOfWeekSprite
{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

enum class TimeSprite
{
    AFTER_SCHOOL_0_0 = 0,
    AFTER_SCHOOL_1_0,
    AFTER_SCHOOL_2_0,
    AFTERNOON_0_0,
    AFTERNOON_1_0,
    AFTERNOON_2_0,
    DAYTIME_0_0,
    DAYTIME_1_0,
    EARLY_MORNING_0_0,
    EARLY_MORNING_1_0,
    EARLY_MORNING_2_0,
    EARLY_MORNING_3_0,
    LATE_NIGHT_0_0,
    LATE_NIGHT_1_0,
    LATE_NIGHT_2_0,
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

enum class DigitSprite
{
    DIGIT_0 = 0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,
    SLASH
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
