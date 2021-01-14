/**
 * shape-config-widget.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#include "shape-config-widget.hpp"

#include <gdkmm/general.h>

namespace brandy0
{

ShapeConfigWidget::ShapeConfigWidget()
{
	set_has_window();
	set_hexpand();
	set_vexpand();
}

constexpr uint32_t MINIMUM_WIDTH = 32;
constexpr uint32_t NATURAL_WIDTH = 512;
constexpr uint32_t MINIMUM_HEIGHT = 32;
constexpr uint32_t NATURAL_HEIGHT = 512;

void ShapeConfigWidget::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
	minimum_width = MINIMUM_WIDTH;
	natural_width = NATURAL_WIDTH;
}

void ShapeConfigWidget::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
	minimum_height = MINIMUM_HEIGHT;
	natural_height = NATURAL_HEIGHT;
}

void ShapeConfigWidget::get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const
{
	minimum_height = MINIMUM_HEIGHT;
	natural_height = NATURAL_HEIGHT;
}

void ShapeConfigWidget::get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const
{
	minimum_width = MINIMUM_WIDTH;
	natural_width = NATURAL_WIDTH;
}

void ShapeConfigWidget::on_size_allocate(Gdk::Rectangle& allocation)
{
	set_allocation(allocation);

	if (win)
	{
		win->move_resize(allocation.get_x(), allocation.get_y(), allocation.get_width(), allocation.get_height());
	}
}

void ShapeConfigWidget::on_realize()
{
	set_realized();

	if (!win)
	{
		GdkWindowAttr attr;
		memset(&attr, 0, sizeof(attr));

		Gtk::Allocation alloc = get_allocation();

		attr.x = alloc.get_x();
		attr.y = alloc.get_y();
		attr.width = alloc.get_width();
		attr.height = alloc.get_height();

		attr.event_mask = get_events() | Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK;
		attr.window_type = GDK_WINDOW_CHILD;
		attr.wclass = GDK_INPUT_OUTPUT;

		win = Gdk::Window::create(get_parent_window(), &attr, GDK_WA_X | GDK_WA_Y);
		set_window(win);

		win->set_user_data(gobj());
		
		signal_button_press_event().connect(sigc::mem_fun(*this, &ShapeConfigWidget::clickHandler), false);
	}
}

void ShapeConfigWidget::on_unrealize()
{
	win.reset();

	Gtk::Widget::on_unrealize();
}

uint32_t ShapeConfigWidget::getWidth() const
{
	return get_allocation().get_width();
}

uint32_t ShapeConfigWidget::getHeight() const
{
	return get_allocation().get_height();
}

bool ShapeConfigWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("light blue"));
	cr->paint();

	const uint32_t sw = getWidth(), sh = getHeight();

	cr->move_to(0, 0);
	cr->line_to(sw, sh);
	cr->move_to(0, sh);
	cr->line_to(sw, 0);
	cr->stroke();

	cr->scale(sw, sh);
	cairo_matrix_t flip;
	cairo_matrix_init_identity(&flip);
	flip.yy = -1;
	cr->transform(flip);
	cr->translate(0, -1);
	if (sw * h > sh * w)
	{
		const double fac = sh * w / (sw * h);
		cr->scale(fac, 1);
		cr->translate((1 / fac - 1) / 2, 0);
	}
	else
	{
		const double fac = sw * h / (sh * w);
		cr->scale(1, fac);
		cr->translate(0, (1 / fac - 1) / 2);
	}

	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("light green"));
	cr->move_to(0, 0);
	cr->line_to(1, 0);
	cr->line_to(1, 1);
	cr->line_to(0, 1);
	cr->close_path();
	cr->fill();

	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("black"));

	for (const std::shared_ptr<ObstacleShape>& shape : shapeStack)
	{
		shape->draw(cr);
	}

	return true;
}

bool ShapeConfigWidget::clickHandler(GdkEventButton *event)
{
	const uint32_t sw = getWidth(), sh = getHeight();
	double rx = event->x / sw, ry = 1 - event->y / sh;
	cout << "click" << endl;
	cout << "wig coors: x = " << rx << ", ry = " << ry << endl;
	if (sw * h > sh * w)
	{
		const double fac = sh * w / (sw * h);
		rx /= fac;
		rx -= (1 / fac - 1) / 2;
	}
	else
	{
		const double fac = sw * h / (sh * w);
		ry /= fac;
		ry -= (1 / fac - 1) / 2;
	}
	cout << "area coors: x = " << rx << ", ry = " << ry << endl;
	nextPolygonVertices.push_back(vec2d(rx, ry));
	return true;
}

void ShapeConfigWidget::submitCurrentPolygon()
{
	shapeStack.push_back(std::make_shared<ObstaclePolygon>(nextPolygonVertices));
	nextPolygonVertices.clear();
}

void ShapeConfigWidget::setWp(const uint32_t wp)
{
	this->wp = wp;
}

void ShapeConfigWidget::setHp(const uint32_t hp)
{
	this->hp = hp;
}

void ShapeConfigWidget::setW(const double w)
{
	this->w = w;
}

void ShapeConfigWidget::setH(const double h)
{
	this->h = h;
}

void ShapeConfigWidget::refresh()
{
	queue_draw();
}

}