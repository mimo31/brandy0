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

#include "obstacle-shape.hpp"

namespace brandy0
{

class ShapeConfigWidget : public Gtk::Widget
{
private:
	uint32_t wp = 64, hp = 64;
	double w = 1, h = 1;

    ObstacleShapeStack shapeStack;

	std::vector<vec2d> nextPolygonVertices;

    Glib::RefPtr<Gdk::Window> win;

    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;


    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;

    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    void on_size_allocate(Gdk::Rectangle& allocation) override;
    void on_realize() override;
    void on_unrealize() override;
    bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	bool clickHandler(GdkEventButton *event);

public:
    ShapeConfigWidget();

	void submitCurrentPolygon();

	void setWp(const uint32_t wp);
	void setHp(const uint32_t hp);
	void setW(const double w);
	void setH(const double h);

	void refresh();
};

}

#endif // SHAPE_CONFIG_WIDGET