/**
 * application-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#ifndef APPLICATION_ABSTR_HPP
#define APPLICATION_ABSTR_HPP

#include <gtkmm/window.h>

#include "graphics.hpp"
#include "simulation-params.hpp"
#include "style-manager.hpp"

namespace brandy0
{

/**
 * An abstract class providing the interface for various states to call methods of the parent application.
 * This is an abstraction of the Application class to provide an interface for its children (states).
 * One of the reasons for this abstraction is that it is not directly possible to include the application header
 * in the headers of the states and also vice versa, but the application object has state objects as members
 * and state objects have a pointer to an application (or just ApplicationAbstr -- that's the solution) object as a member
 */
class ApplicationAbstr
{
public:
	/// The global style manager for the application
	StyleManager styleManager;
	/// The global graphics manager for the application
	GraphicsManager graphicsManager;

	/**
	 * Sets the application state to the configuration state with default simulation parameters
	 */
	virtual void enterNewConfig() = 0;
	/**
	 * Sets the application state to the configuration state and sets particular simulation parameters
	 * @param params the simulation parameters to be set in the newly entered configuration state
	 */
	virtual void enterExistingConfig(SimulationParams params) = 0;
	/**
	 * Sets the application state to the start state
	 */
	virtual void enterHome() = 0;
	/**
	 * Sets the application state to the simulation state. That starts a new simulation.
	 * @param params the simulation parameters for the new simulation
	 */
	virtual void enterNewSimulation(SimulationParams params) = 0;
	/**
	 * Adds a window to the underlying GTK application. Should be called whenever a window that previously hidden is being shown.
	 * @param win the window to add
	 */
	virtual void addWindow(Gtk::Window& win) = 0;
	/**
	 * Starts the underlying GTK application by telling it to run the specified window.
	 * @param win the window to be run
	 */
	virtual void run(Gtk::Window& win) = 0;
};

}

#endif // APPLICATION_ABSTR_HPP