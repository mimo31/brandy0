/**
 * vec2d.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef VEC2D_HPP
#define VEC2D_HPP

#include <cmath>
#include <ostream>

namespace brandy0
{

/**
 * Struct containing two doubles representing a 2D vector
 */
struct vec2d
{
	/// Vector's x coordinate
	double x;
	/// Vector's Y coordinate
	double y;

	vec2d() = default;

	/**
	 * Constructs a vec2d structure given the coordinates of the vector
	 * @param x vector's x coordinate
	 * @param y vector's y coordinate
	 */
	constexpr vec2d(const double x, const double y) : x(x), y(y)
	{
	}

	/**
	 * @param other vector to compare this vector to
	 * @return true iff each of the two components of the vectors are exactly equal
	 */
	constexpr bool operator==(const vec2d &other) const
	{
		return x == other.x && y == other.y;
	}

	/**
	 * @return length squared of this vector (i.e. x^2 + y^2)
	 */
	constexpr double len2() const
	{
		return x * x + y * y;
	}

	/**
	 * @return length of this vector (i.e. sqrt(x^2 + y^2))
	 */
	double len() const
	{
		return sqrt(x * x + y * y);
	}

	/**
	 * @return unit vector in the direction of this vector
	 * This method should not be called on the zero vector.
	 */
	vec2d get_unit() const
	{
		return *this / this->len();
	}

	/**
	 * @return vector perpendicular to this vector which is a 90 degree rotation of this vector to the left
	 * (assuming the positive y axis is 90 to the left of the positive x axis)
	 */
	constexpr vec2d get_lrot() const
	{
		return vec2d(-y, x);
	}

	/**
	 * @return vector perpendicular to this vector which is a 90 degree rotation of this vector to the right
	 * (assuming the positive y axis is 90 to the left of the positive x axis)
	 */
	constexpr vec2d get_rrot() const
	{
		return vec2d(y, -x);
	}

	/**
	 * Adds another vector to this vector component by component
	 * @param other vector to add to this vector
	 * @return reference to this vector
	 */
	constexpr vec2d &operator+=(const vec2d &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	/**
	 * Subtracts another vector from this vector component by component
	 * @param other vector to subtract from this vector
	 * @return reference to this vector
	 */
	constexpr vec2d &operator-=(const vec2d &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	/**
	 * @param other vector to add
	 * @return component by component sum of this vector and the other vector
	 */
	constexpr vec2d operator+(const vec2d &other) const
	{
		return vec2d(x + other.x, y + other.y);
	}

	/**
	 * @param other vector to subtract
	 * @return component by component difference of this vector and the other vector
	 */
	constexpr vec2d operator-(const vec2d &other) const
	{
		return vec2d(x - other.x, y - other.y);
	}

	/**
	 * @param a scalar to multiply by
	 * @return scalar multiple of this vector (multiplied by the specified scalar)
	 */
	constexpr vec2d operator*(const double a) const
	{
		return vec2d(a * x, a * y);
	}

	/**
	 * @param a scalar to multiply by (should not be 0)
	 * @return scalar multiple of this vector (divided by the specified scalar)
	 */
	constexpr vec2d operator/(const double a) const
	{
		return vec2d(x / a, y / a);
	}

	/**
	 * @param other vector to take the dot product with
	 * @return dot product of this vector and the other vector
	 */
	constexpr double dot(const vec2d &other) const
	{
		return x * other.x + y * other.y;
	}

	/**
	 * @param other vector to take the cross product with
	 * @return (last component of the) cross product of this vector and the other vector (positive iff the other vector is to the right of this vector)
	 */
	constexpr double cross(const vec2d &other) const
	{
		return x * other.y - y * other.x;
	}

	/**
	 * @return true iff this vector is exactly the zero vector (both components are exactly 0)
	 */
	constexpr bool is_zero() const
	{
		return x == 0 && y == 0;
	}

	/**
	 * Determines whether this vector is inside a specified rectangle (with sides parallel to the axes)
	 * @param x0 minimum x coordinate of the rectangle
	 * @param y0 minimum y coordinate of the rectangle
	 * @param x1 minimum x coordinate of the rectangle
	 * @param y1 minimum y coordinate of the rectangle
	 * @return true iff this vector is inside the specified rectangle
	 */
	constexpr bool inside(const double x0, const double y0, const double x1, const double y1) const
	{
		return x0 <= x && x <= x1 && y0 <= y && y <= y1;
	}
};

/**
 * @param a scalar to multiply by
 * @param v vector to be multiplied
 * @return scalar multiple of the specified vector (multiplied by the specified scalar)
 */
constexpr vec2d operator*(const double a, const vec2d &v)
{
	return v * a;
}

/**
 * Prints a vector into a stream as "vec2d(x, y)" (with "x", "y" replaced by the values of the components)
 * @param os stream to print into
 * @param v vector to print
 * @return reference to the stream
 */
std::ostream &operator<<(std::ostream &os, const vec2d &v);

}

#endif // VEC2D_HPP
