#pragma once
#include "types/AnimationTypes.hpp"
#include <aegis/ndsTypes.hpp>

#include <aegis/types.hpp>
#include <nds.h>
#include <vector>

/**
 * @brief Loads, animates, and renders a model in the DS geometry engine.
 *
 * Model files provide node hierarchies, animation tracks, and display lists.
 * Texture data is supplied separately by generated model helpers and uploaded
 * to VRAM through this controller.
 */
class AnimationController
{
  public:
    /** @brief Creates the singleton instance if it does not exist. */
    static void create();
    /** @brief Destroys the singleton instance and unloads model textures. */
    static void destroy();
    /** @return The process-wide animation controller instance. */
    static AnimationController* getInstance();

    /**
         * @brief Loads an MDL2 or legacy MDL1 model file.
         * @param filepath Path to the compiled model binary.
         * @return true if the model was opened and parsed successfully.
         */
    bool loadModel(const char* filepath);

    /**
         * @brief Uploads model textures to VRAM after loadModel().
         * @param count Number of texture slots.
         * @param bitmaps Grit-generated bitmap data, one pointer per slot.
         * @param widths Texture widths in pixels.
         * @param heights Texture heights in pixels.
         * @param isRGBA true for RGBA textures, false for RGB textures.
         * @return true after texture slots have been allocated and uploaded.
         * @note Generated model texture helpers supply the required metadata.
         */
    bool loadTextures(
        int count, const unsigned int** bitmaps, const int* widths, const int* heights, const bool* isRGBA);

    /** @brief Deletes all model texture objects from VRAM. */
    void unloadTextures();

    /**
         * @brief Selects an animation.
         * @param animIndex Animation index in the loaded model.
         * @param loop Whether playback should loop when it reaches the end.
         */
    void set(int animIndex, bool loop = true);
    /** @brief Starts the selected animation. */
    void play();
    /** @brief Stops playback and resets the selected animation frame. */
    void stop();
    /** @brief Finishes the current animation at its next boundary. */
    void pause();

    /**
         * @brief Sets animation playback speed.
         * @param speedMultiplier Multiplier relative to the default speed.
         */
    void setAnimationSpeed(ae::q20_12_t speedMultiplier = ae::q20_12_t{1.0})
    {
        animSpeedFP = (int)(speedMultiplier * ae::q20_12_t{128.0});
    }

    /** @brief Advances the selected animation by one frame tick. */
    void update();

    /** @brief Renders the model hierarchy using the current animation frame. */
    void render();

    /** @return true when animation playback is active. */
    bool isAnimationPlaying() const
    {
        return isPlaying;
    }
    /** @return The selected animation index, or -1 when none is selected. */
    int getCurrentAnimIndex() const
    {
        return currentAnimIndex;
    }

  private:
    AnimationController();
    /** @brief Releases texture objects owned by the controller. */
    ~AnimationController();
    static AnimationController* instance;

    void renderNode(int nodeId);
    Keyframe getInterpolatedFrame(const AnimTrack& track, int timeFP, int nodeId);
    static int textureSizeToEnum(int px);

    std::vector<AnimNode> modelNodes;
    std::vector<Animation> animations;
    std::vector<int> trackIndices; // Per-node cached search position.
    std::vector<int> rootNodes;

    // One GL texture ID per texture slot; empty when no textures are loaded.
    std::vector<int> textureIDs;

    int currentAnimIndex = -1;
    int currentFrameFP = 0;
    int animSpeedFP = 128; // 128 = 0.5 frames per tick (converts 30fps to 60fps)
    bool isPlaying = false;
    bool isFinishing = false;
    bool isLooping = true;
};
