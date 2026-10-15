#pragma once

namespace uiAnimation
{

template <typename T> class Animated
{
  public:
    Animated() : value{}
    {
    }
    explicit Animated(T initial) : value(initial)
    {
    }

    Animated(const Animated&) = delete;
    Animated& operator=(const Animated&) = delete;
    Animated(Animated&&) = delete;
    Animated& operator=(Animated&&) = delete;

    T& get()
    {
        return value;
    }
    const T& get() const
    {
        return value;
    }
    void set(const T& v)
    {
        value = v;
    }

    operator T&()
    {
        return value;
    }
    operator const T&() const
    {
        return value;
    }

  private:
    T value;
};

} // namespace uiAnimation
