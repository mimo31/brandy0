/**
 * obstacle-shape.cpp
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

void ObstacleEllipse::draw(const Cairo::RefPtr<Cairo::Context>& cr) const
{
	// TODO: implement
}

/*
ObstacleCircle::ObstacleCircle(const vec2d& center, const double r)
    : ObstacleEllipse(center - vec2d(r, r), center + vec2d(r, r))
{
}*/

ObstaclePolygon::ObstaclePolygon(const std::vector<vec2d>& ps)
    : ps(ps)
{
}

void ObstaclePolygon::draw(const Cairo::RefPtr<Cairo::Context>& cr) const
{
	cr->move_to(ps[0].x, ps[0].y);
	for (uint32_t i = 1; i < ps.size(); i++)
	{
		cr->line_to(ps[i].x, ps[i].y);
	}
	cr->close_path();
	cr->fill();
}

}