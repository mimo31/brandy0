/**
 * display-area.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef DISPLAY_AREA_HPP
#define DISPLAY_AREA_HPP

#include <gtkmm/glarea.h>
#include <giomm/resource.h>
#include <epoxy/gl.h>

#include "display-modes.hpp"
#include "graphics.hpp"
#include "sim-frame.hpp"
#include "simulation-state-abstr.hpp"
#include "simulation-params.hpp"
#include "vec2d.hpp"

namespace brandy0
{

/**
 * The GTK widget used for visualization of computed simulation data using OpenGL
 */
class DisplayArea : public Gtk::GLArea
{
private:
	/// Pointer to the parent (abstract) simulation state
	SimulationStateAbstr *parent;

	/// The OpenGL context used by the widget
	Glib::RefPtr<Gdk::GLContext> glContext;

	/// FrameDrawer used to draw the visualizations
	uptr<FrameDrawer> drawer;

	/**
	 * Initializes the widget. Handler for the realize signal
	 */
	void realize();
	/**
	 * Cleans up before the widget is destroyed. Handler for the unrealize signal
	 */
	void unrealize();
	/**
	 * Draws the visualizations inside the widget. Handler for the render signal
	 */
	bool render(const Glib::RefPtr<Gdk::GLContext>& context);

public:
	/**
	 * Constructs a DisplayArea object
	 * @param parent parent (abstract) simulation state
	 */
	DisplayArea(SimulationStateAbstr *parent);
	/**
	 * Requests a redraw of the widget
	 */
	void redraw();
};

}

#endif // DISPLAY_AREA_HPP
