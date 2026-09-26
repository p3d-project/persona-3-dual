#include "Player.hpp"

bool Player::actorCanUse(ActionBase* action)
{
    return true;
}

void Player::onDead(Event::BattleResult& battleResult)
{
    if (hp > 0)
        return;

    battleResult.playerDied = true;
}
