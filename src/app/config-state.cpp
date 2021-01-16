/**
 * config-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-state.hpp"

namespace brandy0
{

ConfigState::ConfigState(ApplicationAbstr *const app) : app(app)
{
	mainWin = new ConfigWindow(this);
	shapeWin = new ShapeConfigWindow(this);
}

ConfigState::~ConfigState()
{
	delete mainWin;
	delete shapeWin;
}

void ConfigState::activate()
{
	setDefaultParams();
	paramsSwapListeners.invoke();
	showWindows();
}

void ConfigState::activate(const SimulatorParams& params)
{
	setParams(params);
	paramsSwapListeners.invoke();
	showWindows();
}

void ConfigState::showWindows()
{
	app->addWindow(*mainWin);
	mainWin->show();
	app->addWindow(*shapeWin);
	shapeWin->show();

}

void ConfigState::deactivate()
{
	mainWin->hide();
	shapeWin->hide();
	params = nullptr;
}

void ConfigState::setDefaultParams()
{
	const BoundaryCond bc(vec2d(SimulatorParams::DEFAULT_U, SimulatorParams::DEFAULT_U), SimulatorParams::DEFAULT_PRESSURE_BC);
	setParams(SimulatorParams(SimulatorParams::DEFAULT_W, SimulatorParams::DEFAULT_H, SimulatorParams::DEFAULT_WP, SimulatorParams::DEFAULT_HP,
			   	SimulatorParams::DEFAULT_DT, bc, bc, bc, bc, SimulatorParams::DEFAULT_RHO, SimulatorParams::DEFAULT_MU, ObstacleShapeStack(), SimulatorParams::DEFAULT_STOP_AFTER,
				SimulatorParams::DEFAULT_STEPS_PER_FRAME, SimulatorParams::DEFAULT_FRAME_CAPACITY));
}

void ConfigState::setParams(const SimulatorParams& params)
{
	this->params = std::make_unique<SimulatorParams>(params);
}

void ConfigState::submitAll()
{
	app->enterNewSimulation(*params);
}

void ConfigState::goBackHome()
{
	app->enterHome();
}

}
