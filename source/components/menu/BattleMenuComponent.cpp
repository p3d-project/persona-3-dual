#include "BattleMenuComponent.h"
#include "core/globals.h"

BattleMenuComponent* BattleMenuComponent::instance = nullptr;

void BattleMenuComponent::create()
{
    if (instance == nullptr)
    {
        instance = new BattleMenuComponent();
    }
}

void BattleMenuComponent::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

BattleMenuComponent* BattleMenuComponent::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

// option loaders
void BattleMenuComponent::loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name)
{
    // skip if action options have already been loaded
    if (loadedOption == BattleMenuOptions::ACTION)
    {
        return;
    };

    text->clearScreen();

    // set new options
    battleOptions.clear();

    // indicate we loaded option
    loadedOption = BattleMenuOptions::ACTION;
    pauseMessage = name.c_str();
    int count = actions->size();

    for (int i = 0; i < count; i++)
    {
        MenuOption option = {actions->at(i)->name.c_str(), -1, MENU_BIND(BattleMenuComponent, battleOptionSelected)};
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

void BattleMenuComponent::loadSkillOptions(PersonaBase* persona)
{
    // skip if action options have already been loaded
    if (loadedOption == BattleMenuOptions::SKILL)
    {
        return;
    };

    text->clearScreen();

    // set new options
    battleOptions.clear();

    // indicate we loaded option
    loadedOption = BattleMenuOptions::SKILL;
    pauseMessage = "Skills";
    int count = persona->skillCount;

    for (int i = 0; i < count; i++)
    {
        MenuOption option = {
            persona->skills[i]->name.c_str(), -1, MENU_BIND(BattleMenuComponent, battleOptionSelected)};
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

void BattleMenuComponent::loadPersonaOptions(etl::vector<PersonaBase*, 13>* personas)
{
    if (loadedOption == BattleMenuOptions::PERSONA)
    {
        return;
    }

    text->clearScreen();
    battleOptions.clear();
    loadedOption = BattleMenuOptions::PERSONA;
    pauseMessage = "Persona";
    int count = (int)personas->size();

    for (int i = 0; i < count; i++)
    {
        MenuOption option = {personas->at(i)->name.c_str(), -1, MENU_BIND(BattleMenuComponent, battleOptionSelected)};
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

void BattleMenuComponent::loadTargetOptions(etl::vector<BattleParticipant*, 13>* targets, bool healTarget)
{
    BattleMenuOptions targetLoadedOption =
        healTarget ? BattleMenuOptions::TARGET_HEAL : BattleMenuOptions::TARGET_ENEMY;

    if ((loadedOption == BattleMenuOptions::TARGET_HEAL) || (loadedOption == BattleMenuOptions::TARGET_ENEMY))
    {
        return;
    }

    text->clearScreen();
    battleOptions.clear();
    loadedOption = targetLoadedOption;
    pauseMessage = "Target";
    int count = (int)targets->size();

    for (int i = 0; i < count; i++)
    {
        if (targets->at(i)->hp <= 0)
            continue;

        MenuOption option = {targets->at(i)->name.c_str(), -1, MENU_BIND(BattleMenuComponent, battleOptionSelected)};
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = battleOptions.size();
}

void BattleMenuComponent::loadAllOutAttackConfirmation()
{
    if (loadedOption == BattleMenuOptions::ALL_OUT_ATTACK)
    {
        return;
    };

    text->clearScreen();
    battleOptions.clear();
    loadedOption = BattleMenuOptions::ALL_OUT_ATTACK;
    pauseMessage = "Confirm All-out-attack?";

    MenuOption yes = {"Yes", -1, MENU_BIND(BattleMenuComponent, battleOptionSelected)};
    MenuOption no = {"No", -1, MENU_BIND(BattleMenuComponent, battleOptionSelected)};
    battleOptions.push_back(yes);
    battleOptions.push_back(no);

    options = battleOptions.data();
    optionCount = battleOptions.size();
}

void BattleMenuComponent::loadAlertOptions(const std::string& text)
{
    if (loadedOption == BattleMenuOptions::ALERT)
    {
        return;
    }

    battleOptions.clear();
    loadedOption = BattleMenuOptions::ALERT;
    pauseMessage = text;
    alertStartFrame = frame;

    options = nullptr;
    optionCount = 0;
}

bool BattleMenuComponent::isAlertExpired(int durationFrames) const
{
    return (frame - alertStartFrame) >= durationFrames;
}

void BattleMenuComponent::resetHook()
{
    pauseMessage = "";
    options = nullptr;
    optionCount = 0;

    loadedOption = BattleMenuOptions::NONE;
    messagePrinted = false;
    selectedBattleOption = -1;
}

void BattleMenuComponent::resetLoadedOptions()
{
    loadedOption = BattleMenuOptions::NONE;
    messagePrinted = false;
    text->clearScreen();

    pauseMessage = "";

    selectedOption = 0;
    startIndex = 0;
}

ViewState BattleMenuComponent::updateHook()
{
    if (loadedOption == BattleMenuOptions::ALERT)
    {
        if (!messagePrinted)
        {
            text->clearScreen();
            messagePrinted = true;
            text->drawText(pauseMessage, 0, 0, 2);
        }
        return ViewState::KEEP_CURRENT;
    }

    return ViewState::DEFAULT;
}

// option handlers
ViewState BattleMenuComponent::battleOptionSelected()
{
    selectedBattleOption = selectedOption;
    resetLoadedOptions();
    return ViewState::KEEP_CURRENT;
}

int BattleMenuComponent::consumeSelectedBattleOption()
{
    int battleOption = selectedBattleOption;
    selectedBattleOption = -1;
    return battleOption;
}

bool BattleMenuComponent::consumeCancel()
{
    bool result = isCancelled;
    isCancelled = false;
    return result;
}

void BattleMenuComponent::prevOption()
{
    isCancelled = true;
    selectedBattleOption = -1;
    resetLoadedOptions();
}
