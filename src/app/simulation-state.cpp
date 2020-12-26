/**
 * simulation-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#include "simulation-state.hpp"

#include "simulator-classic.hpp"

namespace brandy0
{

SimulationState::SimulationState(Application *const app) : app(app), params(nullptr), sim(nullptr)
{
	win = new SimulationWindow([=](){
		app->enterExistingConfig(*params);
	});
}

SimulationState::~SimulationState()
{
	if (params != nullptr)
		delete params;
	if (sim != nullptr)
		delete sim;
	delete win;
}

void SimulationState::activate(const SimulatorParams& params)
{
	setParams(params);
	win->setParams(this->params);
	sim->iter();
	win->setCurFrame(&(sim->f1));
	showWindow();
}

void SimulationState::deactivate()
{
	win->hide();
}

void SimulationState::showWindow()
{
	app->gtkapp->add_window(*win);
	win->show();
}

void SimulationState::setParams(const SimulatorParams& params)
{
	if (this->params == nullptr)
		this->params = new SimulatorParams(params);
	else
		*(this->params) = params;
	if (this->sim == nullptr)
		this->sim = new SimulatorClassic(params);
}

}
