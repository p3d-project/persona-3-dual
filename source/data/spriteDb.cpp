#include "data/spriteDb.hpp"

static const SpriteDBEntry SPRITE_DB_ENTRY[] = {
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_0), "moon-0"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_1), "moon-1"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_2), "moon-2"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_3), "moon-3"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_4), "moon-4"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_5), "moon-5"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_6), "moon-6"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_7), "moon-7"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_8), "moon-8"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_9), "moon-9"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_10), "moon-10"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_11), "moon-11"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_12), "moon-12"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_13), "moon-13"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_14), "moon-14"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_15), "moon-15"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_16), "moon-16"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_17), "moon-17"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_18), "moon-18"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_19), "moon-19"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_20), "moon-20"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_21), "moon-21"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_22), "moon-22"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_23), "moon-23"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_24), "moon-24"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_25), "moon-25"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_26), "moon-26"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_27), "moon-27"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_28), "moon-28"},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_29), "moon-29"},

    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::SUNDAY), "sunday"},
    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::MONDAY), "monday"},
    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::TUESDAY), "tuesday"},
    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::WEDNESDAY), "wednesday"},
    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::THURSDAY), "thursday"},
    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::FRIDAY), "friday"},
    {SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::SATURDAY), "saturday"},

    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_0), "number-0"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_1), "number-1"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_2), "number-2"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_3), "number-3"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_4), "number-4"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_5), "number-5"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_6), "number-6"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_7), "number-7"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_8), "number-8"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_9), "number-9"},

    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_0_0), "after-school-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_1_0), "after-school-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_2_0), "after-school-2-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_0_0), "afternoon-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_1_0), "afternoon-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_2_0), "afternoon-2-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::DAYTIME_0_0), "daytime-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::DAYTIME_1_0), "daytime-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_0_0), "early-morning-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_1_0), "early-morning-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_2_0), "early-morning-2-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_3_0), "early-morning-3-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_0_0), "late-night-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_1_0), "late-night-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_2_0), "late-night-2-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_0_0), "lunchtime-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_1_0), "lunchtime-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_2_0), "lunchtime-2-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::MORNING_0_0), "morning-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::MORNING_1_0), "morning-1-0"},

    {SpriteType::SKILL_SPRITE, static_cast<int>(SkillSprite::SKILLS_LEVEL), "skills-level"},
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::SLASH), "slash"},

    // Dialogue
    {SpriteType::DIALOGUE, static_cast<int>(DialogueSprite::BLUE_BLOCK), "blue-block"},
    {SpriteType::DIALOGUE, static_cast<int>(DialogueSprite::WHITE_BLOCK), "white-block"},
    {SpriteType::DIALOGUE, static_cast<int>(DialogueSprite::CORNER), "corner"},
    {SpriteType::DIALOGUE, static_cast<int>(DialogueSprite::EDGE), "edge"},
    {SpriteType::DIALOGUE, static_cast<int>(DialogueSprite::CORNER_GREEN), "corner-green"},
    {SpriteType::DIALOGUE, static_cast<int>(DialogueSprite::EDGE_GREEN), "edge-green"},

    // Busts
    {SpriteType::BUST, static_cast<int>(BustSprite::TOP_LEFT), "top-left"},
    {SpriteType::BUST, static_cast<int>(BustSprite::TOP_RIGHT), "top-right"},
    {SpriteType::BUST, static_cast<int>(BustSprite::MIDDLE_LEFT), "middle-left"},
    {SpriteType::BUST, static_cast<int>(BustSprite::MIDDLE_RIGHT), "middle-right"},
    {SpriteType::BUST, static_cast<int>(BustSprite::BOTTOM_LEFT), "bottom-left"},
    {SpriteType::BUST, static_cast<int>(BustSprite::BOTTOM_RIGHT), "bottom-right"},
    // DEBUG
    {SpriteType::BUST, static_cast<int>(BustSprite::EYES_NEUTRAL), "eyes-neutral"},
    {SpriteType::BUST, static_cast<int>(BustSprite::MOUTH_NEUTRAL), "mouth-neutral"},
    {SpriteType::BUST, static_cast<int>(BustSprite::HAPPY), "happy"},
};

static const int SPRITE_DB_ENTRY_LEN = sizeof(SPRITE_DB_ENTRY) / sizeof(SPRITE_DB_ENTRY[0]);

std::string getSpriteFilename(SpriteType type, int id)
{
    for (int i = 0; i < SPRITE_DB_ENTRY_LEN; ++i)
    {
        if (SPRITE_DB_ENTRY[i].type == type && SPRITE_DB_ENTRY[i].id == id)
        {
            return SPRITE_DB_ENTRY[i].filename;
        }
    }
    return "";
}
