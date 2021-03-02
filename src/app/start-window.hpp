/**
 * start-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef START_WINDOW_HPP
#define START_WINDOW_HPP

#include <functional>

#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "brandy-window.hpp"
#include "start-state-abstr.hpp"

namespace brandy0
{

class StartWindow : public BrandyWindow
{
private:
	StartStateAbstr *parent;

	Gtk::Button newButton, aboutButton;
	Gtk::Label descriptionLabel;
	Gtk::Grid layoutGrid;

public:
	StartWindow(StartStateAbstr *const);
};

}

#endif // START_WINDOW_HPP
