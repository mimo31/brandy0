/**
 * config-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#ifndef CONFIG_STATE_ABSTR_HPP
#define CONFIG_STATE_ABSTR_HPP

#include "application-abstr.hpp"
#include "listener-manager.hpp"
#include "ptr.hpp"
#include "simulation-params.hpp"
#include "validator-manager.hpp"

namespace brandy0
{

class ConfigStateAbstr
{
public:
	ApplicationAbstr *app;

	uptr<SimulationParams> params;

	ListenerManager initListeners;
	ListenerManager closeListeners;
	ListenerManager dimensionsChangeListeners;
	ListenerManager shapeStackChangeListeners;
	ListenerManager validityChangeListeners;
	ListenerManager shapeConfigOpenedChangeListeners;
	ListenerManager presetsOpenListeners;
	ListenerManager paramsOverwriteListeners;

	ValidatorManager inputValidators;

	bool shapeConfigOpened;
	bool presetsOpened;

	virtual void submitAll() = 0;
	virtual void goBackHome() = 0;
	virtual void openShapeConfig() = 0;
	virtual void closeAll() = 0;
	virtual void openPresets() = 0;
	virtual void confirmPreset(const SimulationParams &preset) = 0;

	ConfigStateAbstr(ApplicationAbstr *const app);
};

}

#endif // CONFIG_STATE_ABSTR_HPP