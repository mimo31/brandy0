/**
 * simulation-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef SIMULATION_STATE_HPP
#define SIMULATION_STATE_HPP

#include <gtkmm/application.h>

#include "glob.hpp"

#include "application.hpp"
#include "config-window.hpp"
#include "simulation-window.hpp"
#include "simulator.hpp"
#include "simulator-params.hpp"
#include "state.hpp"

namespace brandy0
{

class Application;

class SimulationState : public State
{
private:
	Application *app;
	SimulationWindow *win;
	SimulatorParams *params;
	Simulator *sim;

	void setParams(const SimulatorParams&);
	void showWindow();
public:
	SimulationState(Application *const);
	void activate(const SimulatorParams&);
	void deactivate();
	~SimulationState();
};

}

#endif // SIMULATION_STATE_HPP
