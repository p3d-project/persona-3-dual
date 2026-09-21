#pragma once
#include <aegis/types.hpp>
#include <cstdint>

extern volatile uint32_t frame;

constexpr uint32_t TARGET_FPS = 60;

class Timer
{
  public:
    void start(ae::q20_12_t durationSeconds);
    bool isFinished();
    ae::q20_12_t getElapsed();

  private:
    uint32_t startFrame = 0;
    uint32_t durationFrames = 0;
    bool running = false;
};
