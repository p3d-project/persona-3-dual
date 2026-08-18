#pragma once
#include "battleActions/BattleParticipant.h"
#include "battleActions/actions/ActionBase.h"
#include "battleActions/personas/PersonaBase.h"
#include "components/menu/BattleMenuComponent.h"
#include "components/menu/UIMenu.h"
#include <etl/vector.h>

class BattleMenuComponent : public UIMenu
{
  private:
    friend class BattleSystem;
    BattleMenuComponent() {};
    virtual ~BattleMenuComponent() = default;
    static BattleMenuComponent* instance;

    BattleMenuOptions loadedOption = BattleMenuOptions::NONE;
    int selectedBattleOption = -1;
    bool isCancelled = false;

    etl::vector<MenuOption, 10> battleOptions;
    int alertStartFrame = 0;
    bool messagePrinted = false;

    void resetHook() override;
    void closeHook() override
    {
    }

    // option handlers
    ViewState battleOptionSelected();

  public:
    static void create();
    static void destroy();
    static BattleMenuComponent* getInstance();

    ViewState updateHook() override;
    void prevOption() override;

  protected:
    // helpers
    void resetLoadedOptions();
    int consumeSelectedBattleOption();
    bool consumeCancel();

    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name);
    void loadSkillOptions(PersonaBase* persona);
    void loadPersonaOptions(etl::vector<PersonaBase*, 13>* personas);
    void loadTargetOptions(etl::vector<BattleParticipant*, 13>* targets, bool healTarget);
    void loadAllOutAttackConfirmation();
    void loadAlertOptions(const std::string& text);
    bool isAlertExpired(int durationFrames) const;
};
