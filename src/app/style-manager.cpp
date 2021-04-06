/**
 * style-manager.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/20
 */
#include "style-manager.hpp"

namespace brandy0
{

void StyleManager::requestInit()
{
	if (!initialized)
	{
		redStyle = Gtk::CssProvider::create();
		redStyle->load_from_data("label {color: red}");

		greenStyle = Gtk::CssProvider::create();
		greenStyle->load_from_data("label {color: green}");

		darkGreenStyle = Gtk::CssProvider::create();
		darkGreenStyle->load_from_data("label {color: darkgreen}");
		initialized = true;
	}
}

void StyleManager::setPadding(Gtk::Grid &grid)
{
	Glib::RefPtr<Gtk::CssProvider> style = Gtk::CssProvider::create();
	style->load_from_data("grid { padding: " + std::to_string(StyleManager::WidgetSpacing) + "px }");
	grid.get_style_context()->add_provider(style, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

}