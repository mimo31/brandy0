/**
 * styles.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/2/20
 */
#include "styles.hpp"

namespace brandy0
{

bool Styles::initialized = false;
Glib::RefPtr<Gtk::CssProvider> Styles::redStyle, Styles::greenStyle, Styles::darkGreenStyle;

void Styles::requestInit()
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