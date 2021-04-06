/**
 * vec2d.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/31
 */
#include "vec2d.hpp"

namespace brandy0
{

std::ostream& operator<<(std::ostream& os, const vec2d& v)
{
	os << "vec2d(" << v.x << ", " << v.y << ")";
	return os;
}

}
