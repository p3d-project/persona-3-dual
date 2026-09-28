/**
 * @file timer.hpp
 * @brief Timer util to track if a duration has passed.
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */

#pragma once
#include <aegis/types.hpp>
#include <cstdint>

extern volatile int32_t frame;

constexpr uint32_t TARGET_FPS = 60;

class Timer
{
  public:
    /**
     * @brief Starts a timer.
     *
     * @param durationSeconds How long it takes until the Timer is finished in seconds.
     */
    void start(ae::q20_12_t durationSeconds);

    /**
     * @brief Checks if the timer expired.
     *
     * @return Returns if the timer is finished.
     */
    bool isFinished();

    /**
     * @brief Gets duration of the timer.
     *
     * @return Duration in seconds.
     */
    ae::q20_12_t getDuration();

    /**
     * @brief Gets elapsed time since the timer was started.
     *
     * @return Elapsed time in seconds.
     */
    ae::q20_12_t getElapsed();

    /**
     * @brief Gets progress of how much of the time has passed.
     *
     * @return Progress percentage.
     */
    ae::q20_12_t getProgress();

  private:
    uint32_t startFrame = 0;
    uint32_t durationFrames = 0;
    bool running = false;
};
