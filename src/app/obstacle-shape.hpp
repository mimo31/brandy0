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

#include "grid.hpp"
#include "vec2d.hpp"

namespace brandy0
{

class ObstacleShape
{
protected:
	ObstacleShape(const bool negative);

public:
	bool negative;

	virtual void draw(const Cairo::RefPtr<Cairo::Context>& cr) const = 0;
	virtual void fill(Grid<bool>& grid) const = 0;

};

typedef std::vector<std::shared_ptr<ObstacleShape>>::iterator ObstacleShapeStackIterator;
typedef std::vector<std::shared_ptr<ObstacleShape>>::const_iterator ObstacleShapeStackConstIterator;

class ObstacleShapeStack
{
private:
	uint32_t shownPointer;

public:
	std::vector<std::shared_ptr<ObstacleShape>> shapes;

	ObstacleShapeStack();

	ObstacleShapeStackIterator begin();
	ObstacleShapeStackIterator end();
	ObstacleShapeStackConstIterator begin() const;
	ObstacleShapeStackConstIterator end() const;

	void push(const std::shared_ptr<ObstacleShape> shape);
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

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
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
    ObstaclePolygon(const bool negative, const std::vector<vec2d>& ps);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

}

#endif // OBSTACLE_SHAPE_HPP