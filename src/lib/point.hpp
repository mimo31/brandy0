/**
 * point.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef POINT_HPP
#define POINT_HPP

#include <cstdint>
#include <ostream>

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

	constexpr bool inside(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1) const
	{
		return x0 <= x && x <= x1 && y0 <= y && y <= y1;
	}
};

std::ostream& operator<<(std::ostream& ofs, const Point& p);

}

#endif // POINT_HPP
