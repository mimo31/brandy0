/**
 * obstacle-shape.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#ifndef OBSTACLE_SHAPE_HPP
#define OBSTACLE_SHAPE_HPP

#include <vector>

#include "vec2d.hpp"

namespace brandy0
{

class ObstacleShape
{

virtual void draw(/* TODO some parameters here (where to draw) */) const;

/* TODO add method for selecting squares in the discretization */

};

typedef std::vector<ObstacleShape> ObstacleShapeStack;

class ObstacleEllipse : public ObstacleShape
{
private:
    vec2d p0, p1;

public:
    ObstacleEllipse(const vec2d& p0, const vec2d& p1);

    /* TODO add draw overriden draw methods */
};

class ObstacleCircle : public ObstacleEllipse
{

public:
    ObstacleCircle(const vec2d& center, const double r);
};

class ObstaclePolygon : public ObstacleShape
{
private:
    std::vector<vec2d> ps;

public:
    ObstaclePolygon(const std::vector<vec2d>& ps);

    /* TODO add draw overriden draw methods */
};

}

#endif // OBSTACLE_SHAPE_HPP