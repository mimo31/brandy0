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

/**
 * Class representing the start window
 */
class StartWindow : public BrandyWindow
{
private:
	/// Pointer to the parent (abstract) start state
	StartStateAbstr *parent;

	/// Button for starting (the configuration of) a new simulation
	Gtk::Button newButton;
	/// Button for opening the about window
	Gtk::Button aboutButton;
	/// Label with a welcome message
	Gtk::Label descriptionLabel;
	/// Window's main grid
	Gtk::Grid layoutGrid;

public:
	/**
	 * Constructs a StartWindow object
	 * @param parent pointer to the parent (abstract) start state
	 */
	StartWindow(StartStateAbstr *parent);
};

}

#endif // START_WINDOW_HPP
