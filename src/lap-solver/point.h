/**
 * point.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef POINT_H
#define POINT_H

#include <stdint.h>

namespace brandy0
{

/**
 * Struct containing two integers representing point coordinates
 */
struct Point
{
    int32_t x;
    int32_t y;

    Point()
    {
    }

    constexpr Point(const int32_t x, const int32_t y) : x(x), y(y)
    {
    }
};

}

#endif // POINT_H