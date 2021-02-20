/**
 * styles.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/2/20
 */
#ifndef STYLES_HPP
#define STYLES_HPP

#include <gtkmm/cssprovider.h>

namespace brandy0
{

class Styles
{
	static bool initialized;
public:
	static Glib::RefPtr<Gtk::CssProvider> redStyle, greenStyle, darkGreenStyle;
	static void requestInit();
};

}

#endif // STYLES_HPP