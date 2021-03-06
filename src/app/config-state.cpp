/**
 * config-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-state.hpp"

namespace brandy0
{

ConfigState::ConfigState(ApplicationAbstr *const app)
	: app(app),
	mainWin(std::make_unique<ConfigWindow>(this)),
	shapeWin(std::make_unique<ShapeConfigWindow>(this))
{
}

void ConfigState::activate()
{
	shapeConfigOpened = false;
	setDefaultParams();
	showWindows();
	initListeners.invoke();
}

void ConfigState::activate(const SimulationParams& params)
{
	shapeConfigOpened = false;
	setParams(params);
	showWindows();
	initListeners.invoke();
}

void ConfigState::showWindows()
{
	openShapeConfig();
	app->addWindow(*mainWin);
	mainWin->show();
}

void ConfigState::deactivate()
{
	shapeWin->hide();
	mainWin->hide();
	params = nullptr;
}

void ConfigState::setDefaultParams()
{
	const BoundaryCond bc(vec2d(SimulationParams::DEFAULT_U, SimulationParams::DEFAULT_U), SimulationParams::DEFAULT_PRESSURE_BC);
	setParams(SimulationParams(SimulationParams::DEFAULT_W, SimulationParams::DEFAULT_H, SimulationParams::DEFAULT_WP, SimulationParams::DEFAULT_HP,
			   	SimulationParams::DEFAULT_DT, bc, bc, bc, bc, SimulationParams::DEFAULT_RHO, SimulationParams::DEFAULT_MU, ObstacleShapeStack(), SimulationParams::DEFAULT_STOP_AFTER,
				SimulationParams::DEFAULT_STEPS_PER_FRAME, SimulationParams::DEFAULT_FRAME_CAPACITY));
}

void ConfigState::setParams(const SimulationParams& params)
{
	this->params = std::make_unique<SimulationParams>(params);
}

void ConfigState::submitAll()
{
	app->enterNewSimulation(*params);
	closeListeners.invoke();
}

void ConfigState::goBackHome()
{
	app->enterHome();
	closeListeners.invoke();
}

void ConfigState::openShapeConfig()
{
	if (!shapeConfigOpened)
	{
		app->addWindow(*shapeWin);
		shapeWin->show();
		shapeConfigOpened = true;
		shapeConfigOpenedChangeListeners.invoke();
	}
}

void ConfigState::closeAll()
{
	if (shapeConfigOpened)
		shapeWin->close();
}

}
