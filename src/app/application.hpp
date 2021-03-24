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

#include "application-abstr.hpp"
#include "config-state.hpp"
#include "simulation-state.hpp"
#include "simulation-params.hpp"
#include "state.hpp"
#include "start-state.hpp"

namespace brandy0
{

class Application : public ApplicationAbstr
{
private:
	Glib::RefPtr<Gtk::Application> gtkapp;
	StartState startSt;
	ConfigState configSt;
	SimulationState simulationSt;
	State* activeSt;
	void switchStates(State *const newSt);
public:
	Application();
	void enterNewConfig() override;
	void enterExistingConfig(const SimulationParams) override;
	void enterHome() override;
	void enterNewSimulation(const SimulationParams) override;
	void addWindow(Gtk::Window& win) override;
	void run(Gtk::Window& win) override;
	void run(int argc, const char *const *const argv);
	~Application();
};

}

#endif // APPLICATION_HPP
