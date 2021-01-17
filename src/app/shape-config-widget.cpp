/**
 * shape-config-widget.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#include "shape-config-widget.hpp"

#include <glibmm.h>

#include <gdkmm/general.h>

namespace brandy0
{

ShapeConfigWidget::ShapeConfigWidget(ConfigStateAbstr *parent)
	: parent(parent), mouseInside(false)
{
	set_has_window();
	set_hexpand();
	set_vexpand();

	parent->initListeners.plug([this](){ refresh(); activateRefresher(); });
	parent->closeListeners.plug([this](){ deactivateRefresher(); });
	parent->shapeStackChangeListeners.plug([this](){ refresh(); });
	parent->dimensionsChangeListeners.plug([this](){ refresh(); });
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

void ShapeConfigWidget::get_preferred_height_for_width_vfunc(int /* width */, int& minimum_height, int& natural_height) const
{
	minimum_height = MINIMUM_HEIGHT;
	natural_height = NATURAL_HEIGHT;
}

void ShapeConfigWidget::get_preferred_width_for_height_vfunc(int /* height */, int& minimum_width, int& natural_width) const
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

		attr.event_mask = get_events() | Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::LEAVE_NOTIFY_MASK;
		attr.window_type = GDK_WINDOW_CHILD;
		attr.wclass = GDK_INPUT_OUTPUT;

		win = Gdk::Window::create(get_parent_window(), &attr, GDK_WA_X | GDK_WA_Y);
		set_window(win);

		win->set_user_data(gobj());
		
		signal_button_press_event().connect(sigc::mem_fun(*this, &ShapeConfigWidget::clickHandler), false);
		signal_motion_notify_event().connect(sigc::mem_fun(*this, &ShapeConfigWidget::motionHandler), false);
		signal_leave_notify_event().connect(sigc::mem_fun(*this, &ShapeConfigWidget::leaveHandler), false);
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

void moveTo(const Cairo::RefPtr<Cairo::Context>& cr, const vec2d v)
{
	cr->move_to(v.x, v.y);
}

void lineTo(const Cairo::RefPtr<Cairo::Context>& cr, const vec2d v)
{
	cr->line_to(v.x, v.y);
}

bool ShapeConfigWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->set_source_rgba(1, .9, .9, 1);
	cr->paint();

	const uint32_t sw = getWidth(), sh = getHeight();
	const double w = parent->params->w, h = parent->params->h;
	const uint32_t wp = parent->params->wp, hp = parent->params->hp;

	cr->move_to(0, 0);
	cr->line_to(sw, sh);
	cr->move_to(0, sh);
	cr->line_to(sw, 0);
	cr->stroke();

	cairo_matrix_t flip;
	cairo_matrix_init_identity(&flip);
	flip.yy = -1;
	cr->transform(flip);
	cr->translate(0, -sh);
	cairo_matrix_t baseCoors = cr->get_matrix();
	if (sw * h > sh * w)
	{
		const double fac = sh * w / (sw * h);
		cr->scale(fac, 1);
		cr->translate(sw * (1 / fac - 1) / 2, 0);
	}
	else
	{
		const double fac = sw * h / (sh * w);
		cr->scale(1, fac);
		cr->translate(0, sh * (1 / fac - 1) / 2);
	}
	cr->scale(sw, sh);

	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("light green"));
	cr->move_to(0, 0);
	cr->line_to(1, 0);
	cr->line_to(1, 1);
	cr->line_to(0, 1);
	cr->close_path();
	cr->fill();

	Grid<bool> solid(wp, hp);
	parent->params->shapeStack.set(solid);

	std::vector<vec2d> tempPoly = nextPolygonVertices;
	if (mouseInside)
		tempPoly.push_back(mouseCoors);
	Grid<bool> tempSolid(wp, hp);
	tempSolid.set_all(false);
	if (tempPoly.size() >= 3)
		ObstaclePolygon(false, tempPoly).fill(tempSolid);

	const double dx = 1 / double(wp - 1) / 2;
	const double dy = 1 / double(hp - 1) / 2;
	for (uint32_t x = 0; x < wp; x++)
	{
		for (uint32_t y = 0; y < hp; y++)
		{
			const double xc = x / double(wp - 1);
			const double yc = y / double(hp - 1);
			if (solid(x, y) || (x & 1) != (y & 1))
			{
				if (solid(x, y))
					if ((x & 1) == (y & 1))
						cr->set_source_rgba(.75, .8, 1, 1);
					else
						cr->set_source_rgba(.8, .75, 1, 1);
				else
					Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("light yellow"));
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
			if (!solid(x, y) && tempSolid(x, y))
			{
				if ((x & 1) == (y & 1))
					cr->set_source_rgba(.75, .8, 1, 1);
				else
					cr->set_source_rgba(.8, .75, 1, 1);
				const double x0 = x == 0 ? xc : xc - dx / 2;
				const double x1 = x == wp - 1 ? xc : xc + dx / 2;
				const double y0 = y == 0 ? yc : yc - dy / 2;
				const double y1 = y == hp - 1 ? yc : yc + dy / 2;
				cr->move_to(x0, y0);
				cr->line_to(x0, y1);
				cr->line_to(x1, y1);
				cr->line_to(x1, y0);
				cr->close_path();
				cr->fill();
			}
		}
	}

	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("black"));
	for (const std::shared_ptr<ObstacleShape>& shape : parent->params->shapeStack)
		shape->draw(cr);
	
	cr->set_source_rgba(.5, .5, .5, .5);

	if (tempPoly.size() >= 3)
	{
		const vec2d v0 = tempPoly[0];
		cr->move_to(v0.x, v0.y);
		for (uint32_t i = 0; i < tempPoly.size(); i++)
		{
			const vec2d v = tempPoly[i];
			cr->line_to(v.x, v.y);
		}
		cr->close_path();
		cr->fill();
	}
	else
	{
		const cairo_matrix_t fullCoors = cr->get_matrix();
		cairo_matrix_t baseInv = baseCoors;
		cairo_matrix_invert(&baseInv);
		cairo_matrix_t transMat;
		cairo_matrix_multiply(&transMat, &fullCoors, &baseInv);
		cr->set_matrix(baseCoors);
		const double rad = std::min(sw, sh) / 128.0;
		for (vec2d v : tempPoly)
		{
			cairo_matrix_transform_point(&transMat, &v.x, &v.y);
			cr->move_to(v.x + rad, v.y + rad / 2);
			cr->line_to(v.x + rad, v.y - rad / 2);
			cr->line_to(v.x - rad, v.y + rad / 2);
			cr->line_to(v.x - rad, v.y - rad / 2);
			cr->close_path();
			cr->fill();
		}
		if (tempPoly.size() == 2)
		{
			vec2d v0 = tempPoly[0], v1 = tempPoly[1];
			cairo_matrix_transform_point(&transMat, &v0.x, &v0.y);
			cairo_matrix_transform_point(&transMat, &v1.x, &v1.y);
			const vec2d w = v1 - v0;
			if (!w.is_zero())
			{
				const vec2d sh = w.get_unit().get_lrot() * rad / 8;
				moveTo(cr, v0 + sh);
				lineTo(cr, v1 + sh);
				lineTo(cr, v1 - sh);
				lineTo(cr, v0 - sh);
				cr->close_path();
				cr->fill();
			}
		}
	}

	return true;
}

vec2d ShapeConfigWidget::widgetToUnitCoors(const vec2d wCoors)
{
	const uint32_t sw = getWidth(), sh = getHeight();
	const double w = parent->params->w, h = parent->params->h;
	double rx = wCoors.x / sw, ry = 1 - wCoors.y / sh;
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
	return vec2d(rx, ry);
}

bool ShapeConfigWidget::clickHandler(GdkEventButton *event)
{
	const vec2d uc = widgetToUnitCoors(vec2d(event->x, event->y));
	if (uc.inside(0, 0, 1, 1))
	{
		nextPolygonVertices.push_back(uc);
		refresh();
	}
	return true;
}

bool ShapeConfigWidget::motionHandler(GdkEventMotion *event)
{
	const vec2d uc = widgetToUnitCoors(vec2d(event->x, event->y));
	mouseInside = uc.inside(0, 0, 1, 1);
	if (mouseInside)
		mouseCoors = uc;
	return true;
}

bool ShapeConfigWidget::leaveHandler(GdkEventCrossing * /*event*/)
{
	mouseInside = false;
	return true;
}

void ShapeConfigWidget::activateRefresher()
{
	redrawConnection = Glib::signal_timeout().connect([this](){ refresh(); return true; }, 17);
}

void ShapeConfigWidget::deactivateRefresher()
{
	redrawConnection.disconnect();
}

void ShapeConfigWidget::refresh()
{
	queue_draw();
}

void ShapeConfigWidget::submitCurrentPolygon()
{
	parent->params->shapeStack.push(std::make_shared<ObstaclePolygon>(false, nextPolygonVertices));
	nextPolygonVertices.clear();
	parent->shapeStackChangeListeners.invoke();
}

}