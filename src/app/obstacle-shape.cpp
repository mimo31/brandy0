/**
 * obstcle-shape.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#include "obstacle-shape.hpp"

namespace brandy0
{

ObstacleEllipse::ObstacleEllipse(const vec2d& p0, const vec2d& p1)
    : p0(p0), p1(p1)
{
}

ObstacleCircle::ObstacleCircle(const vec2d& center, const double r)
    : ObstacleEllipse(center - vec2d(r, r), center + vec2d(r, r))
{
}

ObstaclePolygon::ObstaclePolygon(const std::vector<vec2d>& ps)
    : ps(ps)
{
}

}