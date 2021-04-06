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

/**
 * Mode of the shape configuration window for adding a shape
 */
struct AddShapeMode
{
	/// Name of the mode
	str name;

	/**
	 * Constructs an AddShapeMode
	 * @param name name of the mode
	 */
	AddShapeMode(const str name) : name(name)
	{
	}
};

/// Array of all AddShapeMode supported by the shape configuration window
const std::array<AddShapeMode, 4> AddShapeModes{
	AddShapeMode("rectangle"), AddShapeMode("polygon"), AddShapeMode("circle"), AddShapeMode("ellipse")
};
/// Index of the AddShapeMode for adding a rectangle
constexpr uint32_t AddShapeRectangle = 0;
/// Index of the AddShapeMode for adding an arbitrary polygon
constexpr uint32_t AddShapePolygon = 1;
/// Index of the AddShapeMode for adding a circle
constexpr uint32_t AddShapeCircle = 2;
/// Index of the AddShapeMode for adding an ellipse
constexpr uint32_t AddShapeEllipse = 3;
/// Index of the default AddShapeMode
constexpr uint32_t AddShapeModeDefault = AddShapeRectangle;

/**
 * Abstract class representing the shape of an obstacle in the simulated container
 */
class ObstacleShape
{
protected:
	/**
	 * Constructs the ObstacleShape
	 * @param negative true iff the shape is negative @see ObstacleShape::negative
	 */
	ObstacleShape(bool negative);

public:
	/// True iff this shape doesn't make grid points inside of it solid, but instead makes them not solid if they were solid due to some previous shape
	bool negative;

	/**
	 * Draws the shape. Draws into the (0, 0) -- (1, 1) square (transform coordinates beforehand to draw as needed).
	 * @param cr context used for drawing
	 */
	virtual void draw(const Cairo::RefPtr<Cairo::Context>& cr) const = 0;
	/**
	 * Applies this shape to the provided grid by setting the solidness of the grid points inside this shape.
	 * For the purposes of the coordinate transform, assumes the grid is the (0, 0) -- (1, 1) square.
	 * @param grid grid to modify
	 */
	virtual void fill(Grid<bool> &grid) const = 0;

};

typedef vec<sptr<ObstacleShape>>::iterator ObstacleShapeStackIterator;
typedef vec<sptr<ObstacleShape>>::const_iterator ObstacleShapeStackConstIterator;

/**
 * A stack of obstacle shapes with an index of the topmost shown shape.
 * Aallows for undo -- decrement index, redo -- increment index, add shape -- pop down to index & push new shape
 */
class ObstacleShapeStack
{
private:
	/// The number of shown shapes (the first shownPointer shapes from the bottom of the stack are shown)
	uint32_t shownPointer = 0;

public:
	/// Stack of all the shapes (possibly even some which are not shown (@see shownPointer))
	vec<sptr<ObstacleShape>> shapes;
	
	/**
	 * Constructs an empty shape stack
	 */
	ObstacleShapeStack() = default;
	/**
	 * Constructs a shape stack with given shapes in which all shapes are shown
	 */
	ObstacleShapeStack(const vec<sptr<ObstacleShape>> &shapes);

	/**
	 * @return begin (bottom) iterator for the segment of shown shapes
	 */
	ObstacleShapeStackIterator begin();
	/**
	 * @return end (towards top) iterator for the segment of shown shapes
	 */
	ObstacleShapeStackIterator end();
	/**
	 * @return begin (bottom) const iterator for the segment of shown shapes
	 */
	ObstacleShapeStackConstIterator begin() const;
	/**
	 * @return end (towards top) iterator for the segment of shown shapes
	 */
	ObstacleShapeStackConstIterator end() const;

	/**
	 * Adds a shape to the stack and shows it, irreversibly removing all shapes that were in the stack but were not shown
	 */
	void push(const sptr<ObstacleShape> &shape);
	/**
	 * Decrements the number of shown shapes by one, but does not irreversibly remove the now newly hidden shape.
	 * Does nothing if canUndo were to return false (@see canUndo).
	 * @see redo for the inverse operation
	 */
	void undo();
	/**
	 * Increments the number of shown shapes by one by showing a shape that has previously been hidden (through undo).
	 * Does nothing if canRedo were to return false (@see canRedo).
	 * @see undo for the inverse operation
	 */
	void redo();
	/**
	 * Irreversibly removes all shapes
	 */
	void clear();
	/**
	 * @return true iff there is at least one shown shape, i.e., it makes sense to undo its addition
	 * @see undo
	 */
	bool canUndo() const;
	/**
	 * @return true iff there is at least one shape present in this stack but hidden due to an earlier undo operation, i.e., it is possible to redo its addition
	 * @see redo
	 */
	bool canRedo() const;
	/**
	 * @return true iff this stack contains at least one shape (no matter if it's hidden or shown)
	 */
	bool empty() const;

	/**
	 * Applies all the shown shapes to a grid
	 * @param grid grid to be modified
	 */
	void fill(Grid<bool>& grid) const;
	/**
	 * Sets all points of a grid based on what shapes of this stack they are contained in.
	 * @param grid grid to be set
	 * Equivalent to setting all points of the grid to false and then calling this->fill(grid)
	 */
	void set(Grid<bool>& grid) const;
};

/**
 * Obstacle shape representing an ellipse (with axes parallel to the x and y axes)
 */
class ObstacleEllipse : public ObstacleShape
{
private:
	/// Coordinates of the center of this ellipse
	vec2d center;
	/// Length of the semiaxis parallel to the x axis
	double xhaxis;
	/// Length of the semiaxis parallel to the y axis
	double yhaxis;

public:
	/**
	 * Constructs an ObstacleEllipse object representing an ellipse inscribed in a rectangle
	 * @param negative true iff the ellipse should be negative
	 * @param p0 first corner of the circumscribed rectangle
	 * @param p1 second corner of the circumscribed rectangle
	 */
	ObstacleEllipse(bool negative, const vec2d& p0, const vec2d& p1);
	/**
	 * Constrcuts an ObstacleEllipse object given the center and semiaxis lengths
	 * @param negative true iff the ellipse should be negative
	 * @param center coordinates of the center point
	 * @param xhaxis length of the semiaxis parallel to the x axis
	 * @param yhaxis length of the semiaxis parallel to the y axis
	 */
	ObstacleEllipse(bool negative, const vec2d& center, double xhaxis, double yhaxis);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

/**
 * Obstacle shape representing an arbitrary polygon.
 * The interior of the polygon is defined by the non-zero winding rule.
 */
class ObstaclePolygon : public ObstacleShape
{
friend Tests;
private:
	/// Ordered list of this polygon's vertices
	vec<vec2d> ps;

	/**
	 * Implements the non-zero winding rule
	 * @param p point to investigate
	 * @return true iff the specified point is inside the polygon
	 */
	bool inside(vec2d p) const;

public:
	/**
	 * Constructs an ObstaclePolygon object
	 * @param negative true iff the polygon should be negative
	 * @param ps ordered list of the polygon's vertices
	 */
	ObstaclePolygon(bool negative, const vec<vec2d>& ps);

	void draw(const Cairo::RefPtr<Cairo::Context>& cr) const override;
	void fill(Grid<bool>& grid) const override;
};

/**
 * Obstacle shape representing a rectangle with sides parallel to the x and y axes
 */
class ObstacleRectangle : public ObstaclePolygon
{
public:
	/**
	 * Constructs an ObstacleRectangle object
	 * @param negative true iff the rectangle should be negative
	 * @param v0 coordinates of one vertex
	 * @param v1 coordinates of the vertex opposite to v0
	 */
	ObstacleRectangle(bool negative, vec2d v0, vec2d v1);
};

/**
 * Obstacle shape representing a circle
 */
class ObstacleCircle : public ObstacleEllipse
{
public:
	/**
	 * Constructs an ObstacleCircle object
	 * @param negative true iff the circle should be negative
	 * @param center coordinates of the circle center
	 * @param r radius of the circle
	 */
	ObstacleCircle(bool negative, vec2d center, double r);
	/**
	 * Constructs an ObstacleCircle object
	 * @param negative true iff the circle should be negative
	 * @param center coordinates of the circle center
	 * @param v1 coordinates of some point on the boundary of the circle
	 * @param physW physical width of the container in which the shape is a circle (instead of a general ellipse)
	 * @param physH physical height of the container in which the shape is a circle (instread of a general ellipse)
	 */
	ObstacleCircle(bool negative, vec2d center, vec2d v1, double physW, double physH);
};

}

#endif // OBSTACLE_SHAPE_HPP