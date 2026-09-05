#pragma once
#include "views/EnvironmentView.hpp"

#include "data/environmentDb.hpp"
#include "maps/iwatodai_streets.hpp"

#include "battleActions/BattleParticipant.hpp"
#include "battleActions/BattleStartCondition.hpp"
#include "battleActions/enemies/Enemy.hpp"
#include "battleActions/enemies/EnemyProfileDb.hpp"
#include "battleActions/party/CharacterProfileDb.hpp"
#include "battleActions/party/PartyMember.hpp"
#include "battleActions/party/Player.hpp"

#include <etl/vector.h>

class IwatodaiStreetsView : public EnvironmentView
{
  public:
    IwatodaiStreetsView();

    ~IwatodaiStreetsView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[1];
    }

    void setupMovement() override;

    void setupMusic() override;

    ViewState onTileCheck(TileType tile, u32 pressed) override;

    void setupCamera() override;

    void setupText() override;

    void setupUI() override;

    // battle hook
    void startBattle() override;

  private:
    // movement and camera
    const Point2D<ae::q20_12_t> characterSize{ae::q20_12_t{0.1}, ae::q20_12_t{0.1}};
    const ae::q20_12_t speed{0.03};

    // character position
    const Point2D<ae::q20_12_t> characterTranslate{ae::q20_12_t{0.6}, ae::q20_12_t{0.6}};
    const ae::q20_12_t height{0.05};
    const ae::q20_12_t characterFacingAngle{0};

    // battle
    etl::vector<CharacterProfile, 4> characterProfiles;
    etl::vector<EnemyProfile, 8> enemyProfiles;

    BattleStartCondition battleStartCondition = BattleStartCondition::Even;
};
