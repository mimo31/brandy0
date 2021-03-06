/**
 * config-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/16
 */
#ifndef CONFIG_STATE_ABSTR_HPP
#define CONFIG_STATE_ABSTR_HPP

#include "listener-manager.hpp"
#include "ptr.hpp"
#include "simulation-params.hpp"
#include "validator-manager.hpp"

namespace brandy0
{

class ConfigStateAbstr
{
public:
	uptr<SimulationParams> params;

	ListenerManager initListeners;
	ListenerManager closeListeners;
	ListenerManager dimensionsChangeListeners;
	ListenerManager shapeStackChangeListeners;
	ListenerManager validityChangeListeners;
	ListenerManager shapeConfigOpenedChangeListeners;

	ValidatorManager inputValidators;

	bool shapeConfigOpened;

	virtual void submitAll() = 0;
	virtual void goBackHome() = 0;
	virtual void openShapeConfig() = 0;
	virtual void closeAll() = 0;
};

}

#endif // CONFIG_STATE_ABSTR_HPP