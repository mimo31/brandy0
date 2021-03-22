/**
 * obstacle-shape.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#include "obstacle-shape.hpp"

#include <cmath>

namespace brandy0
{

ObstacleShape::ObstacleShape(const bool negative) : negative(negative)
{
}

ObstacleShapeStack::ObstacleShapeStack(const vec<sptr<ObstacleShape>> &shapes)
	: shownPointer(shapes.size()), shapes(shapes)
{
}

ObstacleShapeStackIterator ObstacleShapeStack::begin()
{
	return shapes.begin();
}

ObstacleShapeStackIterator ObstacleShapeStack::end()
{
	return shapes.begin() + shownPointer;
}

ObstacleShapeStackConstIterator ObstacleShapeStack::begin() const
{
	return shapes.begin();
}

ObstacleShapeStackConstIterator ObstacleShapeStack::end() const
{
	return shapes.begin() + shownPointer;
}

void ObstacleShapeStack::push(const std::shared_ptr<ObstacleShape> &shape)
{
	while (shapes.size() > shownPointer)
		shapes.pop_back();
	shapes.push_back(shape);
	shownPointer++;
}

void ObstacleShapeStack::undo()
{
	if (shownPointer != 0)
		shownPointer--;
}

void ObstacleShapeStack::redo()
{
	if (shownPointer < shapes.size())
		shownPointer++;
}

void ObstacleShapeStack::clear()
{
	shapes.clear();
	shownPointer = 0;
}

bool ObstacleShapeStack::canUndo() const
{
	return shownPointer != 0;
}

bool ObstacleShapeStack::canRedo() const
{
	return shownPointer != shapes.size();
}

bool ObstacleShapeStack::empty() const
{
	return shapes.empty();
}

void ObstacleShapeStack::fill(Grid<bool>& grid) const
{
	for (const sptr<ObstacleShape>& shape : *this)
		shape->fill(grid);
}

void ObstacleShapeStack::set(Grid<bool>& grid) const
{
	grid.set_all(false);
	for (const sptr<ObstacleShape>& shape : *this)
		shape->fill(grid);
}

ObstacleEllipse::ObstacleEllipse(const bool negative, const vec2d& p0, const vec2d& p1)
    : ObstacleShape(negative), center((p0 + p1) / 2),
	xhaxis((std::max(p0.x, p1.x) - std::min(p0.x, p1.x)) / 2),
	yhaxis((std::max(p0.y, p1.y) - std::min(p0.y, p1.y)) / 2)
{
}

ObstacleEllipse::ObstacleEllipse(const bool negative, const vec2d& center, const double xhaxis, const double yhaxis)
	: ObstacleShape(negative), center(center), xhaxis(xhaxis), yhaxis(yhaxis)
{
}

void ObstacleEllipse::fill(Grid<bool>& grid) const
{
	for (uint32_t y = 0; y < grid.h; y++)
	{
		for (uint32_t x = 0; x < grid.w; x++)
		{
			const vec2d crs(x / double(grid.w - 1), y / double(grid.h - 1));
			const vec2d rel = crs - center;
			const bool inside = (rel.x / xhaxis) * (rel.x / xhaxis) + (rel.y / yhaxis) * (rel.y / yhaxis) < 1;
			grid(x, y) = grid(x, y) || inside;
		}
	}
}

void ObstacleEllipse::draw(const Cairo::RefPtr<Cairo::Context>& cr) const
{
	if (xhaxis != 0 && yhaxis != 0)
	{
		const cairo_matrix_t origMat = cr->get_matrix();
		cr->translate(center.x, center.y);
		cr->scale(xhaxis, yhaxis);
		cr->arc(0, 0, 1, 0, 2 * M_PI);
		cr->fill();
		cr->set_matrix(origMat);
	}
}

bool onSegment(const vec2d v0, const vec2d v1, const vec2d p)
{
	const vec2d v = v1 - v0;
	const vec2d pv = p - v0;
	const double dot = v.dot(pv);
	return dot * dot == v.len2() * pv.len2() && dot >= 0 && pv.len2() <= v.len2();
}

bool ObstaclePolygon::inside(const vec2d p) const
{
	// return true if p is coincides with one of the vertices
	for (const vec2d v : ps)
	{
		if (p == v)
			return true;
	}

	// return true if p lies on one of the sides
	if (onSegment(ps.front(), ps.back(), p))
		return true;
	for (uint32_t i = 0; i < ps.size() - 1; i++)
	{
		if (onSegment(ps[i], ps[i + 1], p))
			return true;
	}

	// check whether p is in the interior
	int32_t steps = 0;
	auto on_left = [p](const vec2d v) -> bool { return v.x < p.x || (v.x == p.x && v.y < p.y); };
	auto add_segment = [&steps, on_left, p](const vec2d v0, const vec2d v1) -> void
	{
		if (on_left(v0) != on_left(v1))
		{
			const vec2d v = v1 - v0;
			const vec2d pv = p - v0;
			if (v.cross(pv) > 0)
				steps++;
			else
				steps--;
		}
	};
	add_segment(ps.back(), ps.front());
	for (uint32_t i = 0; i < ps.size() - 1; i++)
		add_segment(ps[i], ps[i + 1]);
	return steps != 0;
}

ObstaclePolygon::ObstaclePolygon(const bool negative, const vec<vec2d>& ps)
    : ObstacleShape(negative), ps(ps)
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

void ObstaclePolygon::fill(Grid<bool>& grid) const
{
	for (uint32_t y = 0; y < grid.h; y++)
	{
		for (uint32_t x = 0; x < grid.w; x++)
		{
			const vec2d crs(x / double(grid.w - 1), y / double(grid.h - 1));
			grid(x, y) = grid(x, y) || inside(crs);
		}
	}
}

ObstacleRectangle::ObstacleRectangle(const bool negative, const vec2d v0, const vec2d v1)
	: ObstaclePolygon(negative, vec<vec2d>{ v0, vec2d(v0.x, v1.y), v1, vec2d(v1.x, v0.y) })
{
}

ObstacleCircle::ObstacleCircle(const bool negative, const vec2d center, const double r)
	: ObstacleEllipse(negative, center, r, r)
{
}

ObstacleCircle::ObstacleCircle(const bool negative, const vec2d center, const vec2d v1, const double physW, const double physH)
	: ObstacleEllipse(negative, center,
		sqrt((v1 - center).x * (v1 - center).x * physW * physW + (v1 - center).y * (v1 - center).y * physH * physH) / physW,
		sqrt((v1 - center).x * (v1 - center).x * physW * physW + (v1 - center).y * (v1 - center).y * physH * physH) / physH)
{
}

}