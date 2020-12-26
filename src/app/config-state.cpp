/**
 * config-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-state.hpp"

namespace brandy0
{

ConfigState::ConfigState(Application *const app) : app(app), params(nullptr)
{
	win = new ConfigWindow([=](){
			app->enterHome();
		}, [=](){
			app->enterNewSimulation(*params);
		});
}

ConfigState::~ConfigState()
{
	if (params != nullptr)
		delete params;
	delete win;
}

void ConfigState::activate()
{
	setDefaultParams();
	win->setEntryFields();
	showWindow();
}

void ConfigState::activate(const SimulatorParams& params)
{
	if (this->params == nullptr)
		this->params = new SimulatorParams(params);
	else
		*(this->params) = params;
	win->setEntryFields();
	showWindow();
}

void ConfigState::showWindow()
{
	app->gtkapp->add_window(*win);
	win->show();
}

void ConfigState::deactivate()
{
	win->hide();
}

void ConfigState::setDefaultParams()
{
	Grid<bool> solid(64, 64);
	solid.set_all(false);
	const BoundaryCond bc(vec2d(0, 0), PressureBoundaryCond::VON_NEUMANN);
	setParams(SimulatorParams(SimulatorParams::DEFAULT_W, SimulatorParams::DEFAULT_H, SimulatorParams::DEFAULT_WP, SimulatorParams::DEFAULT_WP,
			   	SimulatorParams::DEFAULT_DT, bc, bc, bc, bc, SimulatorParams::DEFAULT_RHO, SimulatorParams::DEFAULT_MU, solid, SimulatorParams::DEFAULT_STOP_AFTER,
				SimulatorParams::DEFAULT_STEPS_PER_FRAME, SimulatorParams::DEFAULT_FRAME_CAPACITY));
}

void ConfigState::setParams(const SimulatorParams& params)
{
	if (this->params == nullptr)
	{
		this->params = new SimulatorParams(params);
		win->setParamsLocation(this->params);
		// TODO set param location for the solid config window too
	}
	else
		*(this->params) = params;
}

}
