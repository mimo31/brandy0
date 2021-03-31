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

#include <array>

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

const std::array<AddShapeMode, 4> AddShapeModes{
	AddShapeMode("rectangle"), AddShapeMode("polygon"), AddShapeMode("circle"), AddShapeMode("ellipse")
};
constexpr uint32_t AddShapeRectangle = 0;
constexpr uint32_t AddShapePolygon = 1;
constexpr uint32_t AddShapeCircle = 2;
constexpr uint32_t AddShapeEllipse = 3;
constexpr uint32_t AddShapeModeDefault = AddShapeRectangle;

class ObstacleShape
{
protected:
	ObstacleShape(bool negative);

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
    ObstacleEllipse(bool negative, const vec2d& p0, const vec2d& p1);
	ObstacleEllipse(bool negative, const vec2d& center, double xhaxis, double yhaxis);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

class ObstaclePolygon : public ObstacleShape
{
friend Tests;
private:
    vec<vec2d> ps;

	bool inside(vec2d p) const;

public:
    ObstaclePolygon(bool negative, const vec<vec2d>& ps);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

class ObstacleRectangle : public ObstaclePolygon
{
public:
	ObstacleRectangle(bool negative, vec2d v0, vec2d v1);
};

class ObstacleCircle : public ObstacleEllipse
{
public:
	ObstacleCircle(bool negative, vec2d center, double r);
	ObstacleCircle(bool negative, vec2d center, vec2d v1, double physW, double physH);
};

}

#endif // OBSTACLE_SHAPE_HPP