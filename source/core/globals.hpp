/**
 * @file globals.hpp
 * @brief Holds global variables used game-wide
 */

#pragma once
#include <string>

#include "components/DialogueComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/TextComponent.hpp"

#include "managers/AudioManager.hpp"
#include "managers/IOManager.hpp"
#include "managers/MathManager.hpp"
#include "managers/RenderManager.hpp"

#include "systems/BattleSystem.hpp"
#include "systems/CameraSystem.hpp"
#include "systems/SaveSystem.hpp"
#include "systems/UISystem.hpp"

#include "types/SaveTypes.hpp"
#include "types/StateTypes.hpp"

#include <aegis/engine.hpp>

// variables
extern volatile int frame;
extern ViewState nextView;
extern volatile u32 systemKeysDown;
extern volatile u32 systemKeysHeld;
extern std::string fatBasePath;
extern Save saveData;

class Globals
{
  public:
    static bool enableBillboards;
    static bool enableDebugPrint;
    static bool enableCharacterAnim;
    static bool isPauseMenuActive;
};

// aegis engine
namespace GameEngineConfig
{
using LargestMessage = etl::largest_type<Event::BattleResult,
                                         Event::ExecuteBattle,
                                         Event::SetCharacterPosition,
                                         Event::CameraPosition,
                                         Event::ConfigureCamera,
                                         Event::SetCameraMode,
                                         Event::SetCameraPath,
                                         MovementComponent,
                                         DialogueComponent,
                                         Event::ReadSave,
                                         Event::WriteSave,
                                         GraphicsComponent,
                                         TextComponent,
                                         Event::ConfigureUIScreen,
                                         Event::ShowScreen,
                                         Event::HideAllScreens,
                                         Event::ConfigureUIMenu,
                                         Event::HideAllMenus,
                                         Event::RenderUIText,
                                         Event::ResetUIResources>;
constexpr std::size_t kLargestComponentSize = sizeof(typename LargestMessage::type);
constexpr std::size_t kLargestComponentAlign = alignof(typename LargestMessage::type);
} // namespace GameEngineConfig

using GameEngine = ae::Engine<GameEngineConfig::kLargestComponentSize, GameEngineConfig::kLargestComponentAlign>;

extern GameEngine engine;
extern ae::Entity* player;
