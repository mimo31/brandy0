/**
 * brandy-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#ifndef BRANDY_WINDOW_HPP
#define BRANDY_WINDOW_HPP

#include <gtkmm/window.h>

namespace brandy0
{

/**
 * The base class for our program's windows.
 * Doesn't do much; only sets a few default values (such as the title)
 */
class BrandyWindow : public Gtk::Window
{
protected:
	/// Constructs the default brandy0 window
	BrandyWindow();
	/**
	 * Constructs the default brandy0 window with a particular default size.
	 * @param defaultWidth the default width of the window
	 * @param defaultHeight the default height of the window
	 */
	BrandyWindow(uint32_t defaultWidth, uint32_t defaultHeight);
};

}

#endif // BRANDY_WINDOW_HPP