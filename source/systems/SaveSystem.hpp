/**
 * @file SaveSystem.hpp
 * @brief Manages game saves
 *
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "core/routerIDs.hpp"
#include "events/SaveEvents.hpp"
#include "managers/IOManager.hpp"
#include "types/SaveTypes.hpp"

#include <aegis/ndsTypes.hpp>
#include <aegis/system.hpp>
#include <aegis/types.hpp>

class SaveSystem : public ae::SystemRouter<SaveSystem, Event::ReadSave, Event::WriteSave>,
                   public ae::Singleton<SaveSystem>
{
  public:
    void Init() override
    {
    }

    void Shutdown() override
    {
    }

    void Update(ae::q20_12_t /*dt*/) override
    {
    }

    /**
     * @brief ETL message handler to trigger a save read
     *
     * @param msg The event to trigger a read
     */
    void on_receive(const Event::ReadSave /*msg*/);

    /**
     * @brief ETL message handler to trigger a save write
     *
     * @param msg The event to trigger a write
     */
    void on_receive(const Event::WriteSave /*msg*/);

    /**
     * @brief Fallback handler for unhandled ETL messages.
     *
     * @details Required by the ETL message router interface. Safely ignores
     * any messages routed to the SaveSystem that do not have a specific handler.
     *
     * @param msg The unhandled incoming message (unused).
     */
    void on_receive_unknown(const etl::imessage& msg)
    {
    }

  private:
    friend class Singleton<SaveSystem>;
    SaveSystem() : SystemRouter(kSaveSystemRouterID)
    {
    }

    IOManager& io = IOManager::GetInstance();
};
