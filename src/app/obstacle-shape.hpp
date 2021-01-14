/**
 * obstacle-shape.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#ifndef OBSTACLE_SHAPE_HPP
#define OBSTACLE_SHAPE_HPP

#include <memory>
#include <vector>

#include <cairomm/context.h>

#include "glob.hpp"

#include "vec2d.hpp"

namespace brandy0
{

class ObstacleShape
{
public:
	virtual void draw(const Cairo::RefPtr<Cairo::Context>& cr) const = 0;

/* TODO add method for selecting squares in the discretization */

};

typedef std::vector<std::shared_ptr<ObstacleShape>> ObstacleShapeStack;

class ObstacleEllipse : public ObstacleShape
{
private:
    vec2d p0, p1;

public:
    ObstacleEllipse(const vec2d& p0, const vec2d& p1);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;

    /* TODO add draw overriden draw methods */
};

/*class ObstacleCircle : public ObstacleEllipse
{

public:
    ObstacleCircle(const vec2d& center, const double r);
};*/

class ObstaclePolygon : public ObstacleShape
{
private:
    std::vector<vec2d> ps;

public:
    ObstaclePolygon(const std::vector<vec2d>& ps);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;

    /* TODO add draw overriden draw methods */
};

}

#endif // OBSTACLE_SHAPE_HPP