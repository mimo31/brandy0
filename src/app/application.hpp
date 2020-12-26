/**
 * application.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <gtkmm/application.h>

#include "glob.hpp"

#include "config-state.hpp"
#include "simulation-state.hpp"
#include "simulator-params.hpp"
#include "state.hpp"
#include "start-state.hpp"

namespace brandy0
{

class StartState;
class ConfigState;
class SimulationState;

class Application
{
private:
	StartState* startSt;
	ConfigState* configSt;
	SimulationState* simulationSt;
	State* activeSt;
	void switchStates(State *const newSt);
public:
	Glib::RefPtr<Gtk::Application> gtkapp;
	Application();
	void enterNewConfig();
	void enterExistingConfig(const SimulatorParams&);
	void enterHome();
	void enterNewSimulation(const SimulatorParams&);
	void run();
	~Application();
};

}

#endif // APPLICATION_HPP
