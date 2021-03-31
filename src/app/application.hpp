/**
 * application.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <gtkmm/application.h>

#include "application-abstr.hpp"
#include "config-state.hpp"
#include "simulation-state.hpp"
#include "simulation-params.hpp"
#include "state.hpp"
#include "start-state.hpp"

namespace brandy0
{

/**
 * A class containing a managing the top-level of our application.
 * Primarily, it contains all the states of the application and switches between them
 * and it communicates with the underlying GTK application.
 */
class Application : public ApplicationAbstr
{
private:
	/// The underlying GTK application for our application
	Glib::RefPtr<Gtk::Application> gtkapp;
	/// The start state
	StartState startSt;
	/// The configuration state
	ConfigState configSt;
	/// The simulation state
	SimulationState simulationSt;
	/// Pointer to the currently active state (either the start, the configuration, or the simulation state)
	State* activeSt;
	/**
	 * Switches to a new state by calling deactivate on the current state and setting the specified state as active
	 * @param newSt a pointer to the new state
	 */
	void switchStates(State *newSt);
public:
	/**
	 * Constructs the application and its states. Does not run the application. @see Application::run(int, const char *const *)
	 */
	Application();
	/**
	 * Runs the application based on what command-line arguments were given.
	 * @param argc the number of command-line arguments (the argc as in the main function)
	 * @param argv the array of command-line arguments (the argv as in the main function)
	 */
	void run(int argc, const char *const *argv);

	void enterNewConfig() override;
	void enterExistingConfig(SimulationParams) override;
	void enterHome() override;
	void enterNewSimulation(SimulationParams) override;
	void addWindow(Gtk::Window& win) override;
	void run(Gtk::Window& win) override;

	~Application();
};

}

#endif // APPLICATION_HPP
