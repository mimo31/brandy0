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

#include "glob.hpp"

#include "display-modes.hpp"
#include "graphics.hpp"
#include "sim-frame.hpp"
#include "simulation-state-abstr.hpp"
#include "simulation-params.hpp"
#include "vec2d.hpp"

namespace brandy0
{

class DisplayArea : public Gtk::GLArea
{
private:
	SimulationStateAbstr *parent;

	Glib::RefPtr<Gdk::GLContext> glContext;

	std::unique_ptr<FrameDrawer> drawer;

	void realize();
	void unrealize();
	bool render(const Glib::RefPtr<Gdk::GLContext>& context);

public:
	DisplayArea(SimulationStateAbstr *const parent);
	void redraw();
};

}

#endif // DISPLAY_AREA_HPP
