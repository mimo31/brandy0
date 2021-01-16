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
		}, [this](){
			shapeWin->writeObstaclesToParams();
			this->app->enterNewSimulation(*params);
		}, [this](){
			shapeWin->refreshGridSize();
		});
	shapeWin = new ShapeConfigWindow([this](const bool shapeConfigValid) {
		win->setShapeConfigValid(shapeConfigValid);
	});
}

ConfigState::~ConfigState()
{
	if (params != nullptr)
		delete params;
	delete win;
	delete shapeWin;
}

void ConfigState::activate()
{
	setDefaultParams();
	win->setEntryFields();
	shapeWin->setFromParams();
	showWindows();
}

void ConfigState::activate(const SimulatorParams& params)
{
	if (this->params == nullptr)
		this->params = new SimulatorParams(params);
	else
		*(this->params) = params;
	win->setEntryFields();
	shapeWin->setFromParams();
	showWindows();
}

void ConfigState::showWindows()
{
	app->gtkapp->add_window(*win);
	win->show();
	app->gtkapp->add_window(*shapeWin);
	shapeWin->show();

}

void ConfigState::deactivate()
{
	win->hide();
	shapeWin->hide();
}

void ConfigState::setDefaultParams()
{
	/*Grid<bool> solid(SimulatorParams::DEFAULT_WP, SimulatorParams::DEFAULT_HP);
	solid.set_all(false);*/
	const BoundaryCond bc(vec2d(SimulatorParams::DEFAULT_U, SimulatorParams::DEFAULT_U), SimulatorParams::DEFAULT_PRESSURE_BC);
	setParams(SimulatorParams(SimulatorParams::DEFAULT_W, SimulatorParams::DEFAULT_H, SimulatorParams::DEFAULT_WP, SimulatorParams::DEFAULT_HP,
			   	SimulatorParams::DEFAULT_DT, bc, bc, bc, bc, SimulatorParams::DEFAULT_RHO, SimulatorParams::DEFAULT_MU, ObstacleShapeStack(), SimulatorParams::DEFAULT_STOP_AFTER,
				SimulatorParams::DEFAULT_STEPS_PER_FRAME, SimulatorParams::DEFAULT_FRAME_CAPACITY));
}

void ConfigState::setParams(const SimulatorParams& params)
{
	if (this->params == nullptr)
	{
		this->params = new SimulatorParams(params);
		win->setParamsLocation(this->params);
		shapeWin->setParamsLocation(this->params);
	}
	else
		*(this->params) = params;
}

}
