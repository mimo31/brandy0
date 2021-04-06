/**
 * shape-config-widget.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#ifndef SHAPE_CONFIG_WIDGET
#define SHAPE_CONFIG_WIDGET

#include <gtkmm/widget.h>

#include "config-state-abstr.hpp"
#include "obstacle-shape.hpp"
#include "shape-config-window-abstr.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

/**
 * Class representing the widget for configuring and displaying the obstacle shapes
 */
class ShapeConfigWidget : public Gtk::Widget
{
private:
	/// Parent GDK window
	Glib::RefPtr<Gdk::Window> win;

	/// Pointer to the parent (abstract) configuration state
	ConfigStateAbstr *parent;
	/// Pointer to the parent (abstract) shape configuration window
	ShapeConfigWindowAbstr *parentWindow;

	/**
	 * Connection to the callback periodically requesting widget redraws
	 * @see activateRefresher
	 * @see deactivateRefresher
	 */
	sigc::connection redrawConnection;

	/// Coordinates of the mouse in the normalized coordinate system of the container (e.g. (0, 0) -- (1, 1) is the container's diagonal)
	vec2d mouseCoors;
	/// True iff the mouse is currently inside (above) the container (inside the widget is necessary but not sufficient)
	bool mouseInside;

	void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
	void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
	void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
	void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
	void on_size_allocate(Gdk::Rectangle& allocation) override;
	void on_realize() override;
	void on_unrealize() override;

	/**
	 * Draws marks at the specified points.
	 * @param cr context for drawing with coordinates in which (0, 0) -- (1, 1) is the container
	 * @param baseCoors original coordinate matrix (used to ensure that the marks are of correct shape and size)
	 * @param ps points (in (0, 0) -- (1, 1) ~ the container coordinates) to draw marks at
	 * Sets cr's coordinate matrix to baseCoors.
	 */
	void markPoints(const Cairo::RefPtr<Cairo::Context>& cr, const cairo_matrix_t& baseCoors, const vec<vec2d> &ps) const;
	/**
	 * Draws a mark at a specified point.
	 * Equivalent to markPoints(cr, baseCoors, vec<vec2d>{ p }).
	 */
	void markPoint(const Cairo::RefPtr<Cairo::Context>& cr, const cairo_matrix_t& baseCoors, vec2d p) const;
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	/**
	 * @return width of the widget in pixels
	 */
	uint32_t getWidth() const;
	/**
	 * @return height of the widget in pixels
	 */
	uint32_t getHeight() const;

	/**
	 * @param wCoors coordinates of a point with respect to the widget
	 * @return coordinates of a point in the system in which (0, 0) -- (1, 1) is the physical container
	 */
	vec2d widgetToUnitCoors(vec2d wCoors) const;

	/**
	 * Handler of the mouse click event.
	 * To be connected to button_press event.
	 */
	bool clickHandler(GdkEventButton *event);
	/**
	 * Handler of the mouse move event.
	 * To be connected to the motion_notify event.
	 */
	bool motionHandler(GdkEventMotion *event);
	/**
	 * Handler of the mouse leave event.
	 * To be connected to the leave_notify event.
	 */
	bool leaveHandler(GdkEventCrossing *event);

	/**
	 * Starts a timer that periodically refreshes (requests a redraw of) the widget
	 * @see redrawConnection
	 * @see deactivateRefresher
	 */
	void activateRefresher();
	/**
	 * Stops the refreshing timer
	 * @see redrawConnection
	 * @see activateRefresher
	 */
	void deactivateRefresher();

	/**
	 * Requests a redraw of the widget
	 */
	void refresh();

public:
	/**
	 * Constructs a ShapeConfigWidget object
	 * @param parent pointer to the parent (abstract) configuration state
	 * @param parentWindow pointer to the parent (abstract) shape configuration window
	 */
	ShapeConfigWidget(ConfigStateAbstr *parent, ShapeConfigWindowAbstr *parentWindow);
};

}

#endif // SHAPE_CONFIG_WIDGET