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
	/// Point's x coordinate
	int32_t x;
	/// Point's y coordinate
	int32_t y;

	Point() = default;

	/**
	 * Constructs a Point structure given the coordinates of the point
	 * @param x point's x coordinate
	 * @param y point's y coordinate
	 */
	constexpr Point(const int32_t x, const int32_t y) : x(x), y(y)
	{
	}

	/**
	 * Determines whether this point is inside a specified rectangle (with sides parallel to the axes).
	 * Bounds of the rectangle specified as arguments to this method are taken to also be included inside the rectangle.
	 * @param x0 minimum x coordinate of the rectangle
	 * @param y0 minimum y coordinate of the rectangle
	 * @param x1 maximum x coordinate of the rectangle
	 * @param y1 maximum y coordinate of the rectangle
	 * @return true iff this point is inside the specified rectangle
	 */
	constexpr bool inside(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1) const
	{
		return x0 <= x && x <= x1 && y0 <= y && y <= y1;
	}
};

/**
 * Prints a point into a stream as "Point({x}, {y})" (with "x", "y" replaced by the coordinates of the point)
 * @param os stream to print into
 * @param p point to print
 * @return reference to the stream
 */
std::ostream &operator<<(std::ostream &os, const Point &p);

}

#endif // POINT_HPP
