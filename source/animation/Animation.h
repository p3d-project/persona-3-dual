#pragma once

namespace uiAnimation
{

class Animation
{
  public:
    virtual ~Animation() = default;

    bool tick(float dt)
    {
        if (done || cancelled)
            return true;
        if (!paused)
            done = onUpdate(dt);
        return done;
    }

    virtual void reverse()
    {
    }

    void pause()
    {
        paused = true;
    }
    void resume()
    {
        paused = false;
    }
    void cancel()
    {
        cancelled = true;
    }

    bool isCancelled() const
    {
        return cancelled;
    }
    bool isPaused() const
    {
        return paused;
    }
    bool isDone() const
    {
        return done;
    }

  protected:
    virtual bool onUpdate(float dt) = 0;

    bool paused = false;
    bool cancelled = false;
    bool done = false;
};

} // namespace uiAnimation
