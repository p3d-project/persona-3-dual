#pragma once

#include <aegis/system.hpp>

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
    HideAllMenus,
    RenderUIText,
    ResetUIResources
};
} // namespace EventID

enum class ComponentType : ae::ComponentTypeID
{
    None = 0,
    Movement,
    Dialogue,
    Graphics,
    Text,
    Music,
    SFX
};
