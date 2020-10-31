/**
 * vec2d.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/31
 */
#include "vec2d.hpp"

namespace brandy0
{

vec2d operator*(const double a, const vec2d& v)
{
	return v * a;
}

std::ostream& operator<<(std::ostream& os, const vec2d& v)
{
	os << "vec2d(" << v.x << ", " << v.y << ")";
	return os;
}

}
