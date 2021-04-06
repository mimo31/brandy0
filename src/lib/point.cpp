/**
 * point.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/31
 */
#include "point.hpp"

namespace brandy0
{

std::ostream &operator<<(std::ostream &os, const Point &p)
{
	os << "Point(" << p.x << ", " << p.y << ")";
	return os;
}

}
