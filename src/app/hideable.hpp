/**
 * hideable.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/18
 */
#ifndef HIDEABLE_HPP
#define HIDEABLE_HPP

#include <cairomm/context.h>
#include <cairomm/refptr.h>

namespace brandy0
{

/**
 * Template for a hideable version of a GTK+ widget.
 * In constrast to the functionality provided by Gtk::Widget::hide, Gtk::Widget::show,
 * this (templated) class hides / shows the widget while preserving its space requirements (thereby not letting other widgets rearrange),
 * i.e., in GTK+'s sense the widget remains shown at all times.
 * T represents the original widget type; it should derive from Gtk::Widget.
 */
template<typename T>
class Hideable : public T
{
private:
	/// True iff currently hidden
	bool hidden = false;

	bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override
	{
		// draw the original widget iff !hidden. Else draw nothing

		if (!hidden)
			return T::on_draw(cr);
		return true;
	}

public:
	/**
	 * Constructs the hideable widget by calling the parent constructor with the same arguments
	 */
	template<typename... CtorArgs>
	Hideable(CtorArgs... args) : T(args...)
	{
	}

	/**
	 * Hides this widget without altering its space requirements.
	 * Does nothing iff this widget is already hidden by this method.
	 * @see pseudoShow for unhiding
	 */	
	void pseudoHide()
	{
		hidden = true;
		T::set_sensitive(false);
	}

	/**
	 * Shows this widget without altering its space requirements.
	 * Does nothing iff this widget is not already hidden by pseudoHide.
	 * @see pseudoHide for hiding
	 */
	void pseudoShow()
	{
		hidden = false;
		T::set_sensitive(true);
	}
};

}

#endif // HIDEABLE_HPP