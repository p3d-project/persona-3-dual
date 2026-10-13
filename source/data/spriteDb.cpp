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

    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_0_0), "after-school-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_1_0), "after-school-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_0_0), "afternoon-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_1_0), "afternoon-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::DAYTIME_0_0), "daytime-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::DAYTIME_1_0), "daytime-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_0_0), "early-morning-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_1_0), "early-morning-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_0_0), "late-night-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_1_0), "late-night-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_0_0), "lunchtime-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_1_0), "lunchtime-1-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_2_0), "lunchtime-2-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::MORNING_0_0), "morning-0-0"},
    {SpriteType::TIME, static_cast<int>(TimeSprite::MORNING_1_0), "morning-1-0"},

    {SpriteType::SKILL_SPRITE, static_cast<int>(SkillSprite::SKILLS_LEVEL), "skills-level"},

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
