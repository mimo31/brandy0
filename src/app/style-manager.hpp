/**
 * style-manager.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/20
 */
#ifndef STYLE_MANAGER_HPP
#define STYLE_MANAGER_HPP

#include <gtkmm/cssprovider.h>
#include <gtkmm/grid.h>

namespace brandy0
{

class StyleManager
{
	bool initialized = false;
public:
	Glib::RefPtr<Gtk::CssProvider> redStyle, greenStyle, darkGreenStyle;
	void requestInit();

	static constexpr uint32_t WidgetSpacing = 5;
	
	static void setPadding(Gtk::Grid &grid);
};

}

#endif // STYLE_MANAGER_HPP