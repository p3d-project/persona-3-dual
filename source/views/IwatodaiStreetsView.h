#pragma once

#include "views/EnvironmentView.h"

// data
#include "data/environmentDb.h"
// maps
#include "maps/iwatodai_streets.h"
// battle
#include "./battleActions/BattleParticipant.h"
#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/enemies/EnemyProfileDb.h"
#include "./battleActions/party/CharacterProfileDb.h"
#include "./battleActions/party/PartyMember.h"
#include "./battleActions/party/Player.h"

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

    void setMovementConfig() override;

    void setMusic() override;

    ViewState onTileCheck(TileType tile, u32 pressed) override;

    void setCameraConfig() override;

    void setTextConfig() override;

    void setupUI() override;

    // battle hook
    void startBattle() override;

  private:
    // movement and camera
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;

    // character position
    const Point2D<float> characterTranslate = Point2D<float>(0.60f, 0.60f);
    const float height = 0.05f;
    const float characterFacingAngle = 0.0f;

    // battle
    etl::vector<CharacterProfile, 4> characterProfiles;
    etl::vector<EnemyProfile, 8> enemyProfiles;

    BattleStartCondition battleStartCondition = BattleStartCondition::Even;
};
