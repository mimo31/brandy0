/**
 * shape-config-widget.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#ifndef SHAPE_CONFIG_WIDGET
#define SHAPE_CONFIG_WIDGET

#include <gtkmm/widget.h>

#include "glob.hpp"

#include "config-state-abstr.hpp"
#include "obstacle-shape.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class ShapeConfigWidget : public Gtk::Widget
{
private:
	std::vector<vec2d> nextPolygonVertices;

    Glib::RefPtr<Gdk::Window> win;

	ConfigStateAbstr *parent;

    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    void on_size_allocate(Gdk::Rectangle& allocation) override;
    void on_realize() override;
    void on_unrealize() override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	bool clickHandler(GdkEventButton *event);

	void refresh();

public:
    ShapeConfigWidget(ConfigStateAbstr *parent);

	void submitCurrentPolygon();
};

}

#endif // SHAPE_CONFIG_WIDGET