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
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "annotated-entry.hpp"
#include "bc-selector.hpp"
#include "brandy-window.hpp"
#include "config-state-abstr.hpp"
#include "hideable.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

/**
 * Class represeting the main configuration window.
 */
class ConfigWindow : public BrandyWindow
{
private:
	/// Label at the top of the window describing what this window is about
	Gtk::Label descriptionLabel;
	
	/// Entry for the fluid density (rho)
	AnnotatedEntry rhoEntry;
	/// Entry for the fluid (dynamic) viscosity (mu)
	AnnotatedEntry muEntry;
	/// Entries for the left (x = 0) boundary condition
	BCSelector x0sel;
	/// Entries for the right (x = width) boundary condition
	BCSelector x1sel;
	/// Entries for the bottom (y = 0) boundary condition
	BCSelector y0sel;
	/// Entries for the top (y = height) boundary condition
	BCSelector y1sel;

	/// Entry for the grid width (number of grid points along the x axis)
	AnnotatedEntry gridWidthEntry;
	/// Entry for the grid height (number of grid points along the y axis)
	AnnotatedEntry gridHeightEntry;
	/// Entry for simulator time step (dt)
	AnnotatedEntry dtEntry;
	/// Entry for the number of steps (iterations) in one (base) frame
	AnnotatedEntry stepsPerFrameEntry;
	/// Entry for the maximum number of frames that can be stored at once
	AnnotatedEntry frameCapacityEntry;

	/// (TODO: implement) Checkbox to indicate whether the computation of the simulation should automatically pause after some time
	Gtk::CheckButton autoStop;

	/// Main grid for the entire window
	Gtk::Grid rootGrid;

	/// Frame for the entries relating to the physical properties of the modelled fluid (a.k.a. what we model)
	Gtk::Frame physFrame;
	/// Grid for the entries relating to the physical properties of the modelled fluid (@see physFrame)
	Gtk::Grid physGrid;

	/// Frame for the entries relating to the computational parameters of the simulator (how, how accurately,... we model the fluid)
	Gtk::Frame compFrame;
	/// Grid for the entries relating to the computational parameters of the simulator (@see compFrame)
	Gtk::Grid compGrid;

	/// Button for the return to the start state
	Gtk::Button backHomeButton;
	/// Button for submitting the currently entered parameters and starting a simulation
	Gtk::Button startSimButton;
	/// Button for opening the preset window
	Gtk::Button presetButton;
	/// Button for reopening the shape configuration window in case the user has closed the it
	Hideable<Gtk::Button> reopenButton;

	/// Pointer to the parent (abstract) configuration state
	ConfigStateAbstr *parent;

	/**
	 * Connects appropriate handlers to signals and events produced by this window and its widgets
	 * (meant to be called exactly once, in the constructor)
	 */
	void connectWindowEventHandlers();
	/**
	 * Connects appropriate handlers to events managed by the parent state
	 * (meant to be called exactly once, in the constructor)
	 */
	void connectStateEventHandlers();
	/**
	 * @return true iff none of the entry fields in this window have invalid input entered by the user
	 */
	bool areInputsValid() const;
	/**
	 * Sets the contents of the widgets in this window based on the simulation parameters stored in the parent state
	 */
	void setEntryFields();
public:
	/**
	 * Constructs the configuration window object
	 * @param parent pointer to the parent (abstract) configuration state
	 */
	ConfigWindow(ConfigStateAbstr *parent);
};

}

#endif // CONFIG_WINDOW_HPP
