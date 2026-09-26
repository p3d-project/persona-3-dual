/**
 * @file geometry.h
 * @brief Holds geometry structures & operations
 */

#ifndef Geometry_h
#define Geometry_h

template <typename T> struct Point2D
{
    T x;
    T z;

    Point2D() : x(0), z(0)
    {
    }
    Point2D(T x, T z) : x(x), z(z)
    {
    }

    Point2D<T> operator+(const Point2D<T>& other) const
    {
        return Point2D<T>(x + other.x, z + other.z);
    }

    Point2D<T> operator-(const Point2D<T>& other) const
    {
        return Point2D<T>(x - other.x, z - other.z);
    }

    Point2D<T>& operator=(const Point2D<T>& other)
    {
        x = other.x;
        z = other.z;
        return *this;
    }

    bool operator==(const Point2D<T>& other) const
    {
        return other.x == x && other.z == z;
    }

    bool operator!=(const Point2D<T>& other) const
    {
        return !(*this == other);
    }
};

template <typename T> struct Rectangle
{
    Point2D<T> position;
    Point2D<T> size;

    Point2D<T> bottomRight() const
    {
        return this->position + this->size;
    }

    Point2D<T> bottomLeft() const
    {
        return this->position + Point2D<T>(0, size.z);
    }

    Point2D<T> TopLeft() const
    {
        return this->position;
    }

    Point2D<T> TopRight() const
    {
        return this->position + Point2D<T>(size.x, 0);
    }

    T top() const
    {
        return position.z;
    }

    T left() const
    {
        return position.x;
    }

    T bottom() const
    {
        return position.z + size.z;
    }

    T right() const
    {
        return position.x + size.x;
    }

    T width() const
    {
        return size.x;
    }

    T height() const
    {
        return size.z;
    }

    bool checkCollision(const Rectangle<T>& other, bool ignoreTouching = true) const
    {
        if (ignoreTouching)
        {
            return (left() < other.right() && right() > other.left()) &&
                   (top() < other.bottom() && bottom() > other.top());
        }

        return (left() <= other.right() && right() >= other.left()) &&
               (top() <= other.bottom() && bottom() >= other.top());
    }

    bool operator==(const Rectangle<T>& other) const
    {
        return position == other.position && size == other.size;
    }

    bool operator!=(const Rectangle<T>& other) const
    {
        return !(*this == other);
    }
};

template <typename T> struct Vec3
{
    T x;
    T y;
    T z;

    Vec3() : x(0), y(0), z(0)
    {
    }
    Vec3(T x, T y, T z) : x(x), y(y), z(z)
    {
    }

    Vec3<T> operator+(const Vec3<T>& other) const
    {
        return Vec3<T>(x + other.x, y + other.y, z + other.z);
    }

    Vec3<T> operator-(const Vec3<T>& other) const
    {
        return Vec3<T>(x - other.x, y - other.y, z - other.z);
    }

    Vec3<T> operator*(T scalar) const
    {
        return Vec3<T>(x * scalar, y * scalar, z * scalar);
    }

    Vec3<T>& operator+=(const Vec3<T>& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3<T>& operator=(const Vec3<T>& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    bool operator==(const Vec3<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vec3<T>& other) const
    {
        return !(*this == other);
    }

    T dot(const Vec3<T>& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3<T> cross(const Vec3<T>& other) const
    {
        return Vec3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }
};

#endif
