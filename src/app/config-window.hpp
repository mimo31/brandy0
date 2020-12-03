/**
 * config-window.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#ifndef CONFIG_WINDOW_HPP
#define CONFIG_WINDOW_HPP

#include <functional>

#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "glob.h"

namespace brandy0
{

class ConfigWindow : public Gtk::Window
{
private:
	Gtk::Button backHomeButton;
	Gtk::Entry gridWidthEntry;
	Gtk::Label gridWidthLabel;
	Gtk::Grid layoutGrid;
public:
	ConfigWindow(const std::function<void()>& backHomeCallback);
	~ConfigWindow() override;
};

}

#endif // CONFIG_WINDOW_HPP
