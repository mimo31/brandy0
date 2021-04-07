/**
 * config-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-state.hpp"

#include "simulation-params-preset.hpp"

namespace brandy0
{

ConfigState::ConfigState(ApplicationAbstr *const app)
	: ConfigStateAbstr(app),
	mainWin(this),
	shapeWin(this),
	presetWin(this)
{
}

void ConfigState::activate()
{
	activate(SimulationParamsPreset::DefaultParams);
}

void ConfigState::activate(const SimulationParams& params)
{
	shapeConfigOpened = false;
	presetsOpened = false;
	setParams(params);
	showWindows();
	initListeners.invoke();
}

void ConfigState::showWindows()
{
	openShapeConfig();
	app->addWindow(mainWin);
	mainWin.show();
}

void ConfigState::deactivate()
{
	shapeWin.hide();
	mainWin.hide();
	params = nullptr;
}

void ConfigState::setParams(const SimulationParams& params)
{
	this->params = make_unique<SimulationParams>(params);
	paramsOverwriteListeners.invoke();
	dimensionsChangeListeners.invoke();
	shapeStackChangeListeners.invoke();
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
		app->addWindow(shapeWin);
		shapeWin.show();
		shapeConfigOpened = true;
		shapeConfigOpenedChangeListeners.invoke();
	}
}

void ConfigState::closeAll()
{
	if (shapeConfigOpened)
		shapeWin.close();
	if (presetsOpened)
		presetWin.close();
}

void ConfigState::openPresets()
{
	if (!presetsOpened)
	{
		presetsOpened = true;
		presetsOpenListeners.invoke();
		app->addWindow(presetWin);
		presetWin.show();
	}
	else
		presetWin.present();
}

void ConfigState::confirmPreset(const SimulationParams &preset)
{
	setParams(preset);
	presetWin.hide();
	presetsOpened = false;
}

void ConfigState::presetsClosing()
{
	presetsOpened = false;
}

}
