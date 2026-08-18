/**
 * @file enums.h
 * @brief Holds enums used game-wide
 */

#pragma once
#include <aegis/system.hpp>

enum class ViewState
{
    DEFAULT,
    KEEP_CURRENT,
    DISCLAIMER,
    INTRO_VIDEO,
    INTRO,
    MAIN_MENU,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    CUTSCENE_1,
    SIGN_CONTRACT,
    CUTSCENE_2,
    STATION,
    PAULOWNIA_MALL,
};

enum class SpriteType
{
    NONE = 0,
    MOON,
    DAY_OF_WEEK,
    DIGIT,
    TIME,
    SKILL_SPRITE,
    DIALOGUE,
    CUSTOM,
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
    NAME_TAG = 0,
    NAME_TAG_FEMC,
    CALENDAR,
    CALENDAR_FEMC,
    TEXT_CORNER,
    TEXT_CORNER_FEMC,
    TEXT_MIDDLE,
    TEXT_MIDDLE_FEMC,
};

enum class TileType
{
    NO_COLLISION = 0,
    COLLISION = 1,
    SAVE = 2,

    // scenes / events
    SCENE_0 = 400,
    SCENE_1 = 401,
    SCENE_2 = 402,
    SCENE_3 = 403,
    SCENE_4 = 404,
    SCENE_5 = 405,
    SCENE_6 = 406,
    SCENE_7 = 407,
    SCENE_8 = 408,
    SCENE_9 = 409,

    // characters (party)
    C_MC = 600,
    C_YU = 601,
    C_JU = 602,
    C_AK = 603,
    C_MI = 604,
    C_FU = 605,
    C_AE = 606,
    C_KO = 607,
    C_KE = 608,
    C_SH = 609,

    // enemies (shadows)
    // w = weak, m = medium, s = tring
    SHD_W = 800,
    SHD_M = 801,
    SHD_S = 802,
};

enum class MainMenuOptions
{
    LOAD_GAME = 0,
    SETTINGS,
    RETURN_TO_TITLE,
};

enum class LevelOptions
{
    START_GAME = 0,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    STATION,
    PAULOWNIA_MALL,
    SIGN_CONTRACT,
};

enum class SettingIntroOptions
{
    ORIGINAL = 0,
    FES,
    PORTABLE,
    RELOAD
};

enum class SettingOptions
{
    CHANGE_INTRO_VIDEO = 0,
    FEMC_MODE
};

enum class BattleMenuOptions
{
    NONE = 0,
    ACTION,
    SKILL,
    PERSONA,
    TARGET_ENEMY,
    TARGET_HEAL,
    ALL_OUT_ATTACK,
    ALERT
};

enum class PauseMenuOption
{
    SKILL = 0,
    ITEM = 1,
    PERSONA = 2,
    EQUIP = 3,
    STATUS = 4,
    S_LINK = 5,
    SYSTEM = 6
};

enum class Character
{
    MAKOTO = 0,
    YUKARI = 1,
    JUNPEI = 2,
    AKIHIKO = 3,
    MITSURU = 4,
    AIGIS = 5,
    KEN = 6,
    KOROMARU = 7,
    SHINJIRO = 8
};

enum class Item
{
    LIFE_STONE = 0,
    MEDICINE = 1,
    BEAD = 2
};

enum class Persona
{
    JACK_FROST = 0,
    BLACK_FROST = 1,
    KING_FROST = 2
};

enum class SLink
{
    FOOL = 0,
    MAGICIAN = 1,
    EMPEROR = 2
};

enum class SystemOption
{
    TUTORIAL = 0,
    CONFIG = 1,
    DICTIONARY = 2,
    LOAD_DATA = 3,
    SAVE_DATA = 4,
    RETURN_TO_TITLE = 5
};

enum class DebugOption
{
    DISCLAIMER_VIEW = 0,
    INTRO_VIEW,
    MAIN_MENU_VIEW,
    IWATODAI_DORM_VIEW,
    IWATODAI_STREETS_VIEW,
    STATION_VIEW,
    SIGN_CONTRACT_VIEW,
    PAULOWNIA_MALL_VIEW,
    INTRO_VIDEO,
    CUTSCENE_1,
    CUTSCENE_2,
    DEBUG_DIALOGUE,
    TOGGLE_BILLBOARDS,
    TOGGLE_DEBUG_PRINT,
    PLAY_CHARACTER_ANIM,
    CYCLE_CAMERA_MODE
};

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

enum class ViewPhase
{
    Battle,
    Pause,
    Dialogue,
    Environment
};

namespace EventID
{
enum : etl::message_id_t
{
    ExecuteBattle = 0,
    BattleResult,
    CameraPosition,
    ConfigureCamera,
    SetCameraMode,
    SetCameraPath,
    StartCamera,
    StopCamera,
    SetCharacterPosition,
    WriteSave,
    ReadSave,
    ConfigureUIScreen,
    ShowScreen,
    HideAllScreens,
    ConfigureUIMenu,
    SwitchView,
    ShowMenu,
    HideAllMenus
};
} // namespace EventID

enum class ComponentType : ae::ComponentTypeID
{
    None = 0,
    Movement,
    Dialogue,
    Graphics,
    Text
};

/**
 * @brief Controls how the camera behaves each frame.
 *
 * - Free   : first-person fly cam, d-pad moves, L/R rotates.
 * - Static : fixed eye and target, ignores all input.
 * - CCTV   : fixed eye position, target tracks the character.
 * - Follow : orbits behind the character, L/R adjusts orbit angle.
 * - Path   : plays back a @ref CameraPath keyframe sequence, then
 *            automatically returns to Follow when complete.
 */
enum class CameraMode
{
    Free,
    Static,
    CCTV,
    Follow,
    Path
};

/**
 * @brief Human Readable enum for text colors.
 */
enum TextColor
{
    Transparent = 0,
    Black = 1,
    White = 2,
    DualGreen = 3,
    DualGreen2 = 4,
    DarkGreen = 5,
    DarkerGreen = 6,
    DarkestGreen = 7,
    LightBlue = 8,
    RichBlue = 9,
    DarkBlue = 10,
    NavyBlue = 11,
    DarkestBlue = 12,
    LightOrange = 13,
    LightPurple = 14,
    Red = 15,
    Green = 16,
    Blue = 17,
    Yellow = 18,
    Magenta = 19,
    Cyan = 20,
    Gray = 21
};

/**
 * @brief Human readable enum for text instructions.
 */
enum TextInstruction
{
    ColorChange = 0x01,
    StyleChange = 0x02,
    StyleBold = 0x01,
    StyleItalic = 0x02,
    StyleUnderline = 0x04,
    Reset = 0xFF
};
