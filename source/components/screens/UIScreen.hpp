#pragma once
#include "types/UITypes.hpp"
#include <nds.h>
#include <vector>

class UIScreen
{
  public:
    // TODO: ensure bgId, oam is set before calling load()
    int bgId = -1;
    bool isMain = false;
    OamState* oam = nullptr;
    bool isLoaded = false;

    /**
     * @brief Loads graphics data into memory & pushes to VRAM
     */
    virtual void load() = 0;

    /**
     * @brief Unloads graphics data from memory
     */
    virtual void unload() = 0;

    /**
     * @brief Trigger an action to be handled
     */
    virtual void triggerAction(UIAction action);

    /**
     * @brief Applys sprite colour palette & draws sprites to screen
     */
    virtual void renderSprites();

    /**
     * @brief Clears the displayed sprites from the screen
     */
    void removeSprites();

    /**
     * @brief Hook to allow touch response
     *
     * @param touch touch input
     * @return int a value that acts as an id, used by calling code to understand which area has been touched
     */
    virtual int onTouch(touchPosition* touch);
    virtual ~UIScreen() = default;
    UIScreen(bool iIsMain) : isMain(iIsMain)
    {
    }

  protected:
    /**
   * @brief Moves the specified sprite to the specified position on the screen
   *
   * @param spriteId the sprite to move
   * @param x the x position on the screen
   * @param y the y position on the screen
   */
    void moveSprite(int spriteId, int x, int y);

    /**
     * @brief Shows the specified sprite on the screen
     *
     * @param spriteId the sprite to show
     */
    void showSprite(int spriteId);

    /**
     * @brief Hides the specified sprite on the screen
     *
     * @param spriteId the sprite to hide
     */
    void hideSprite(int spriteId);
};
