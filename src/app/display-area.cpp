#include "display-area.hpp"

#include <chrono>

#include "point.hpp"

namespace brandy0
{

DisplayArea::DisplayArea(SimulationStateAbstr *const parent) : parent(parent)
{
	set_hexpand(true);
	set_vexpand(true);
	set_auto_render(true);

	signal_realize().connect(sigc::mem_fun(*this, &DisplayArea::realize), true);
	signal_unrealize().connect(sigc::mem_fun(*this, &DisplayArea::unrealize), false);
	signal_render().connect(sigc::mem_fun(*this, &DisplayArea::render));

	parent->initListeners.plug([this, parent]()
	{
		drawer = std::make_unique<FrameDrawer>(*parent->params);
	});
}

bool DisplayArea::render(const Glib::RefPtr<Gdk::GLContext>& /* context */)
{
	make_current();

	if (glContext)
		glContext->make_current();

	try
	{
		throw_if_error();
		
		drawer->setBackDisplayMode(parent->backDisplayMode);
		drawer->setFrontDisplayMode(parent->frontDisplayMode);
		drawer->drawFrame(*parent->curFrame, get_width(), get_height());

		return true;
	}
	catch (const Gdk::GLError& gle)
	{
		cerr << "An error occurred in the render callback of the GLArea" << endl;
		cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
		return false;
	}
}

void reportError(const Gdk::GLError& gle, const std::string where)
{
	cerr << "make_current on glArea resulted in an error in " << where << endl;
	cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
}

void DisplayArea::realize()
{
	make_current();
	try
	{
		throw_if_error();

		graphics::init();

		glContext = get_context();
		graphics::ctx = glContext;
	}
	catch (const Gdk::GLError& gle)
	{
		reportError(gle, "realize");
	}
}

void DisplayArea::unrealize()
{
	make_current();

	if (glContext)
		glContext->make_current();

	try
	{
		throw_if_error();

		// TODO resolve the below comment
		// delete buffers
	}
	catch (const Gdk::GLError& gle)
	{
		reportError(gle, "unrealize");
	}
}

void DisplayArea::redraw()
{
	queue_render();
}

}
