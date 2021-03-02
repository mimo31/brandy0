/**
 * application.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#include "application.hpp"

namespace brandy0
{

Application::Application()
{
	gtkapp = Gtk::Application::create("");
	startSt = new StartState(this);
	configSt = new ConfigState(this);
	simulationSt = new SimulationState(this);
}

Application::~Application()
{
	activeSt->deactivate();
	graphics::destruct();
	delete startSt;
	delete configSt;
	delete simulationSt;
}

void Application::run()
{
	activeSt = startSt;
	startSt->run();
}

void Application::switchStates(State *const newSt)
{
	activeSt->deactivate();
	activeSt = newSt;
}

void Application::enterNewConfig()
{
	switchStates(configSt);
	configSt->activate();
}

void Application::enterExistingConfig(const SimulatorParams params)
{
	switchStates(configSt);
	configSt->activate(params);
}

void Application::enterHome()
{
	switchStates(startSt);
	startSt->activate();
}

void Application::enterNewSimulation(const SimulatorParams params)
{
	switchStates(simulationSt);
	simulationSt->activate(params);
}

void Application::addWindow(Gtk::Window& win)
{
	gtkapp->add_window(win);
}

void Application::run(Gtk::Window& win)
{
	gtkapp->run(win);
}

}
