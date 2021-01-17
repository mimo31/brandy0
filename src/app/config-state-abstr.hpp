/**
 * config-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/16
 */
#ifndef CONFIG_STATE_ABSTR_HPP
#define CONFIG_STATE_ABSTR_HPP

#include <memory>

#include "listener-manager.hpp"
#include "simulator-params.hpp"
#include "validator-manager.hpp"

namespace brandy0
{

class ConfigStateAbstr
{
public:
	std::unique_ptr<SimulatorParams> params;

	ListenerManager initListeners;
	ListenerManager closeListeners;
	ListenerManager dimensionsChangeListeners;
	ListenerManager shapeStackChangeListeners;
	ListenerManager validityChangeListeners;

	ValidatorManager inputValidators;

	virtual void submitAll() = 0;
	virtual void goBackHome() = 0;
};

}

#endif // CONFIG_STATE_ABSTR_HPP