/**
 * @file DialogueComponent.hpp
 * @brief Orchestrates dialogue display & branching logic.
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "components/TextComponent.hpp"
#include "components/screens/DialogueScreen.hpp"
#include "types/DialogueTypes.hpp"
#include "types/aeTypes.hpp"

#include <aegis/component.hpp>

class DialogueComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Dialogue);
    void Init() override;

    void Destroy() override;

    /**
     * @brief Core update loop that renders & advances through dialogue
     *
     * @param dt Fixed-point delta time passed from the aegis engine loop (currently unused).
     */
    void Update(ae::q20_12_t /*dt*/) override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Configure the dialogue system
     *
     * Required to call before calling start().
     *
     * @param config The struct containing the dialogue configuration to apply.
     */
    void configureDialogue(const DialogueConfig& config);

    /**
     * @brief Start the currently loaded dialogue
     *
     * @note Dialogue must be set by configureDialogue() before start() can be called
     *
     * @param firstLine the first dialogue line
     */
    void start(Dialogue* firstLine);

    /**
     * @brief End the dialogue display
     */
    void end();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    /**
     * @brief Transition to a new Dialogue node and reset animation state
     *
     * Commonly used when decidiing what dialogue to display after a DialogueSelection
     *
     * @param Pointer to the next Dialogue
     */
    void advanceTo(Dialogue* next);

    /**
     * @brief Animate in the dialogue text on appear
     */
    void renderAnimFrame();

    /**
     * @brief Display the DialogueSelection options
     */
    void renderOptions();

    Dialogue* current = nullptr;
    int optionCount = 0;
    int selectedOption = 0;
    bool doRenderOptions = false;

    /// track the currently loaded bust
    bool renderBust = true;

    u32 prevKeys = 0;

    TextComponent* text;
    TextComponent* textAlt;
    DialogueScreen* screen;
};
