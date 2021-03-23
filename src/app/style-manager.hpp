/**
 * style-manager.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/20
 */
#ifndef STYLE_MANAGER_HPP
#define STYLE_MANAGER_HPP

#include <gtkmm/cssprovider.h>

namespace brandy0
{

class StyleManager
{
	bool initialized = false;
public:
	Glib::RefPtr<Gtk::CssProvider> redStyle, greenStyle, darkGreenStyle;
	void requestInit();
};

}

#endif // STYLE_MANAGER_HPP