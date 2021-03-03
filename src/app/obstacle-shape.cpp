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

ObstacleShapeStack::ObstacleShapeStack()
	: shownPointer(0)
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

void ObstacleShapeStack::push(const std::shared_ptr<ObstacleShape> shape)
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
	return shapes.size() == 0;
}

void ObstacleShapeStack::fill(Grid<bool>& grid) const
{
	for (const std::shared_ptr<ObstacleShape>& shape : *this)
		shape->fill(grid);
}

void ObstacleShapeStack::set(Grid<bool>& grid) const
{
	grid.set_all(false);
	for (const std::shared_ptr<ObstacleShape>& shape : *this)
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

ObstaclePolygon::ObstaclePolygon(const bool negative, const std::vector<vec2d>& ps)
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

bool intersects(const vec2d a0, const vec2d a1, const vec2d b0, const vec2d b1)
{
	const vec2d a = a1 - a0;
	const vec2d b0p = b0 - a0;
	const vec2d b1p = b1 - a0;
	const double sab0 = a.cross(b0p);
	const double sab1 = a.cross(b1p);
	if ((sab0 > 0 && sab1 > 0) || (sab0 < 0 && sab1 < 0))
		return false;
	const vec2d b = b1 - b0;
	const vec2d a0p = a0 - b0;
	const vec2d a1p = a1 - b0;
	const double sba0 = b.cross(a0p);
	const double sba1 = b.cross(a1p);
	return !((sba0 > 0 && sba1 > 0) || (sba0 < 0 && sba1 < 0));
}

void ObstaclePolygon::fill(Grid<bool>& grid) const
{
	// TODO: potentially room for improvement (current algorithm may produce incorrect results
	// e.g. when the ray goes through a vertex)
	// TODO: fix discrepancy between polygon fill and draw methods (when two parts of the polygon overlap, fill does not fill that intersection, but it should)
	for (uint32_t y = 0; y < grid.h; y++)
	{
		for (uint32_t x = 0; x < grid.w; x++)
		{
			const vec2d crs(x / double(grid.w - 1), y / double(grid.h - 1));
			bool inside = false;
			for (uint32_t i = 0; i < ps.size(); i++)
			{
				const vec2d v0 = ps[i];
				const vec2d v1 = i + 1 != ps.size() ? ps[i + 1] : ps[0];
				inside = inside ^ intersects(vec2d(-1, -1.3), crs, v0, v1);
			}
			grid(x, y) = grid(x, y) || inside;
		}
	}
}

ObstacleRectangle::ObstacleRectangle(const bool negative, const vec2d v0, const vec2d v1)
	: ObstaclePolygon(negative, std::vector<vec2d>{ v0, vec2d(v0.x, v1.y), v1, vec2d(v1.x, v0.y) })
{
}

ObstacleCircle::ObstacleCircle(const bool negative, const vec2d center, const vec2d v1, const double physW, const double physH)
	: ObstacleEllipse(negative, center,
		sqrt((v1 - center).x * (v1 - center).x * physW * physW + (v1 - center).y * (v1 - center).y * physH * physH) / physW,
		sqrt((v1 - center).x * (v1 - center).x * physW * physW + (v1 - center).y * (v1 - center).y * physH * physH) / physH)
{
}

}