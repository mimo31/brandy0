/**
 * shape-config-widget.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#ifndef SHAPE_CONFIG_WIDGET
#define SHAPE_CONFIG_WIDGET

#include <gtkmm/widget.h>

#include "glob.hpp"

#include "config-state-abstr.hpp"
#include "obstacle-shape.hpp"
#include "shape-config-window-abstr.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

class ShapeConfigWidget : public Gtk::Widget
{
private:
    Glib::RefPtr<Gdk::Window> win;

	ConfigStateAbstr *parent;
	ShapeConfigWindowAbstr *parentWindow;

	sigc::connection redrawConnection;

	vec2d mouseCoors;
	bool mouseInside;

    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    void on_size_allocate(Gdk::Rectangle& allocation) override;
    void on_realize() override;
    void on_unrealize() override;
	void markPoints(const Cairo::RefPtr<Cairo::Context>& cr, const cairo_matrix_t& baseCoors, const vec<vec2d> &ps) const;
	void markPoint(const Cairo::RefPtr<Cairo::Context>& cr, const cairo_matrix_t& baseCoors, const vec2d p) const;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	vec2d widgetToUnitCoors(const vec2d wCoors);

	bool clickHandler(GdkEventButton *const event);
	bool motionHandler(GdkEventMotion *const event);
	bool leaveHandler(GdkEventCrossing *const event);

	void activateRefresher();
	void deactivateRefresher();

	void refresh();

public:
    ShapeConfigWidget(ConfigStateAbstr *const parent, ShapeConfigWindowAbstr *const parentWindow);
};

}

#endif // SHAPE_CONFIG_WIDGET