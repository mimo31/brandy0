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

ShapeConfigWidget::ShapeConfigWidget(const std::function<void()> shapeStackChangedCallback)
	: shapeStackChangedCallback(shapeStackChangedCallback)
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
	//Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("light pink").);
	cr->set_source_rgba(1, .9, .9, 1);
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

	Grid<bool> solid(wp, hp);
	setFromObstacleShapeStack(solid, shapeStack);

	const double dx = 1 / double(wp - 1) / 2;
	const double dy = 1 / double(hp - 1) / 2;
	for (uint32_t x = 0; x < wp; x++)
	{
		for (uint32_t y = 0; y < hp; y++)
		{
			if (!solid(x, y) && (x & 1) == (y & 1))
				continue;
			if (solid(x, y))
				if ((x & 1) == (y & 1))
					cr->set_source_rgba(.75, .8, 1, 1);
				else
					cr->set_source_rgba(.8, .75, 1, 1);
			else
				Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("light yellow"));
			const double xc = x / double(wp - 1);
			const double yc = y / double(hp - 1);
			const double x0 = x == 0 ? xc : xc - dx;
			const double x1 = x == wp - 1 ? xc : xc + dx;
			const double y0 = y == 0 ? yc : yc - dy;
			const double y1 = y == hp - 1 ? yc : yc + dy;
			cr->move_to(x0, y0);
			cr->line_to(x0, y1);
			cr->line_to(x1, y1);
			cr->line_to(x1, y0);
			cr->close_path();
			cr->fill();
		}
	}

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
	if (rx >= 0 && rx <= 1 && ry >= 0 && ry <= 1)
		nextPolygonVertices.push_back(vec2d(rx, ry));
	return true;
}

void ShapeConfigWidget::submitCurrentPolygon()
{
	shapeStack.push(std::make_shared<ObstaclePolygon>(false, nextPolygonVertices));
	nextPolygonVertices.clear();
	shapeStackChangedCallback();
}

void ShapeConfigWidget::undo()
{
	shapeStack.undo();
}

void ShapeConfigWidget::redo()
{
	shapeStack.redo();
}

bool ShapeConfigWidget::canUndo() const
{
	shapeStack.canUndo();
}

bool ShapeConfigWidget::canRedo() const
{
	shapeStack.canRedo();
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

void ShapeConfigWidget::writeShapeStack(SimulatorParams *const params) const
{
	params->shapeStack = shapeStack;
}

}