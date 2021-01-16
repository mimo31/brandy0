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

	double len() const
	{
		return sqrt(x * x + y * y);
	}

	vec2d get_unit() const
	{
		return *this / this->len();
	}

	vec2d get_lrot() const
	{
		return vec2d(-y, x);
	}

	vec2d get_rrot() const
	{
		return vec2d(y, -x);
	}

	vec2d &operator+=(const vec2d &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	vec2d &operator-=(const vec2d &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
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

	vec2d operator/(const double a) const
	{
		return vec2d(x / a, y / a);
	}

	double dot(const vec2d &other) const
	{
		return x * other.x + y * other.y;
	}

	double cross(const vec2d &other) const
	{
		return x * other.y - y * other.x;
	}

	bool is_zero() const
	{
		return x == 0 && y == 0;
	}
};

vec2d operator*(const double a, const vec2d& v);

std::ostream& operator<<(std::ostream& os, const vec2d& v);

}

#endif // VEC2D_HPP
