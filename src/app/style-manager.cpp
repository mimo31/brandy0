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

}