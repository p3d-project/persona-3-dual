#include "timer.hpp"
#include "error.hpp"
#include "managers/MathManager.hpp"

void Timer::start(ae::q20_12_t durationSeconds)
{
    if (durationSeconds.raw_value() < 0)
    {
        logMessage(LogSeverity::WARNING, "Called with negative duration");
        durationSeconds = ae::q20_12_t{0};
    }
    startFrame = frame;
    durationFrames = static_cast<uint32_t>((static_cast<uint64_t>(durationSeconds.raw_value()) * TARGET_FPS) >> 12);
    running = true;
}

bool Timer::isFinished()
{
    if (!running)
    {
        return true;
    }
    return (frame - startFrame) >= durationFrames;
}

ae::q20_12_t Timer::getDuration()
{
    return ae::q20_12_t::from_raw_value(
        static_cast<int32_t>((static_cast<uint64_t>(durationFrames) << 12) / TARGET_FPS));
}

ae::q20_12_t Timer::getElapsed()
{
    uint32_t elapsedFrames = frame - startFrame;
    return ae::q20_12_t::from_raw_value(
        static_cast<int32_t>((static_cast<uint64_t>(elapsedFrames) << 12) / TARGET_FPS));
}

ae::q20_12_t Timer::getProgress()
{
    ae::q20_12_t duration = getDuration();
    if (duration.raw_value() == 0)
    {
        return ae::q20_12_t{1};
    }
    return MathManager::GetInstance().div(getElapsed(), duration);
}
