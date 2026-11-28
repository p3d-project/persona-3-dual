#pragma once

#include <aegis/system.hpp>

namespace EventID
{
inline constexpr etl::message_id_t EXECUTE_BATTLE = 0;
inline constexpr etl::message_id_t BATTLE_RESULT = 1;
inline constexpr etl::message_id_t CAMERA_POSITION = 2;
inline constexpr etl::message_id_t CONFIGURE_CAMERA = 3;
inline constexpr etl::message_id_t SET_CAMERA_MODE = 4;
inline constexpr etl::message_id_t SET_CAMERA_PATH = 5;
inline constexpr etl::message_id_t START_CAMERA = 6;
inline constexpr etl::message_id_t STOP_CAMERA = 7;
inline constexpr etl::message_id_t SET_CHARACTER_POSITION = 8;
inline constexpr etl::message_id_t WRITE_SAVE = 9;
inline constexpr etl::message_id_t READ_SAVE = 10;
inline constexpr etl::message_id_t CONFIGURE_UI_SCREEN = 11;
inline constexpr etl::message_id_t SHOW_SCREEN = 12;
inline constexpr etl::message_id_t HIDE_ALL_SCREENS = 13;
inline constexpr etl::message_id_t CONFIGURE_UI_MENU = 14;
inline constexpr etl::message_id_t SWITCH_VIEW = 15;
inline constexpr etl::message_id_t SHOW_MENU = 16;
inline constexpr etl::message_id_t HIDE_ALL_MENUS = 17;
inline constexpr etl::message_id_t RENDER_UI_TEXT = 18;
inline constexpr etl::message_id_t RESET_UI_RESOURCES = 19;
} // namespace EventID

enum class ComponentType : ae::ComponentTypeID
{
    NONE = 0,
    MOVEMENT,
    DIALOGUE,
    GRAPHICS,
    TEXT,
    MUSIC,
    SFX,
    MODEL,
    ANIMATION
};
