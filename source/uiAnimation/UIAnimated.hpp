/**
 * @file UIAnimated.hpp
 * @brief Non-copyable wrapper around a value that UI tweens can target.
 * @author Oles Gedz (@olesgedz)
 */

#pragma once

/**
 * @brief Owns a value of type T that property animations may write to.
 *
 * Copy and move are deleted so the address passed to tweens stays stable.
 */
template <typename T> class UIAnimated
{
  public:
    UIAnimated() : value{}
    {
    }

    explicit UIAnimated(T initial) : value(initial)
    {
    }

    ~UIAnimated() = default;

    UIAnimated(const UIAnimated&) = delete;
    UIAnimated& operator=(const UIAnimated&) = delete;
    UIAnimated(UIAnimated&&) = delete;
    UIAnimated& operator=(UIAnimated&&) = delete;

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
