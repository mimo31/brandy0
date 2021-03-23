/**
 * obstacle-shape.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#ifndef OBSTACLE_SHAPE_HPP
#define OBSTACLE_SHAPE_HPP

#define _USE_MATH_DEFINES

#include <cairomm/context.h>

#include "glob.hpp"

#include "grid.hpp"
#include "ptr.hpp"
#include "str.hpp"
#include "tests.hpp"
#include "vec.hpp"
#include "vec2d.hpp"

namespace brandy0
{

struct AddShapeMode
{
	str name;

	AddShapeMode(const str name) : name(name)
	{
	}
};

constexpr uint32_t ADD_SHAPE_MODE_COUNT = 4;
const AddShapeMode ADD_SHAPE_MODES[ADD_SHAPE_MODE_COUNT] = {
	AddShapeMode("rectangle"), AddShapeMode("polygon"), AddShapeMode("circle"), AddShapeMode("ellipse")
};
constexpr uint32_t ADD_SHAPE_RECTANGLE = 0;
constexpr uint32_t ADD_SHAPE_POLYGON = 1;
constexpr uint32_t ADD_SHAPE_CIRCLE = 2;
constexpr uint32_t ADD_SHAPE_ELLIPSE = 3;
constexpr uint32_t ADD_SHAPE_MODE_DEFAULT = ADD_SHAPE_RECTANGLE;

class ObstacleShape
{
protected:
	ObstacleShape(const bool negative);

public:
	bool negative;

	virtual void draw(const Cairo::RefPtr<Cairo::Context>& cr) const = 0;
	virtual void fill(Grid<bool>& grid) const = 0;

};

typedef vec<sptr<ObstacleShape>>::iterator ObstacleShapeStackIterator;
typedef vec<sptr<ObstacleShape>>::const_iterator ObstacleShapeStackConstIterator;

class ObstacleShapeStack
{
private:
	uint32_t shownPointer = 0;

public:
	vec<sptr<ObstacleShape>> shapes;

	ObstacleShapeStack() = default;
	ObstacleShapeStack(const vec<sptr<ObstacleShape>> &shapes);

	ObstacleShapeStackIterator begin();
	ObstacleShapeStackIterator end();
	ObstacleShapeStackConstIterator begin() const;
	ObstacleShapeStackConstIterator end() const;

	void push(const sptr<ObstacleShape> &shape);
	void undo();
	void redo();
	void clear();
	bool canUndo() const;
	bool canRedo() const;
	bool empty() const;

	void fill(Grid<bool>& grid) const;
	void set(Grid<bool>& grid) const;
};

class ObstacleEllipse : public ObstacleShape
{
private:
	vec2d center;
	double xhaxis, yhaxis;

public:
    ObstacleEllipse(const bool negative, const vec2d& p0, const vec2d& p1);
	ObstacleEllipse(const bool negative, const vec2d& center, const double xhaxis, const double yhaxis);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

class ObstaclePolygon : public ObstacleShape
{
friend Tests;
private:
    vec<vec2d> ps;

	bool inside(const vec2d p) const;

public:
    ObstaclePolygon(const bool negative, const vec<vec2d>& ps);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

class ObstacleRectangle : public ObstaclePolygon
{
public:
	ObstacleRectangle(const bool negative, const vec2d v0, const vec2d v1);
};

class ObstacleCircle : public ObstacleEllipse
{
public:
	ObstacleCircle(const bool negative, const vec2d center, const double r);
	ObstacleCircle(const bool negative, const vec2d center, const vec2d v1, const double physW, const double physH);
};

}

#endif // OBSTACLE_SHAPE_HPP