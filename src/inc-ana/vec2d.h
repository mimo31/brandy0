/**
 * vec2d.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef VEC2D_H
#define VEC2D_H

namespace brandy0
{

/**
 * Struct containing two doubles representing a 2D vector
 */
struct vec2d
{
    double x;
    double y;

    vec2d()
    {
    }

    constexpr vec2d(const double x, const double y) : x(x), y(y)
    {
    }

    double len2() const
    {
        return x * x + y * y;
    }

    void operator+=(const vec2d &other)
    {
        x += other.x;
        y += other.y;
    }

    vec2d operator+(const vec2d &other) const
    {
        return vec2d(x + other.x, y + other.y);
    }

    vec2d operator-(const vec2d &other) const
    {
        return vec2d(x - other.x, y - other.y);
    }

    vec2d operator*(const double a) const
    {
        return vec2d(a * x, a * y);
    }

    double dot(const vec2d &other) const
    {
        return x * other.x + y * other.y;
    }
};

}

#endif // VEC2D_H