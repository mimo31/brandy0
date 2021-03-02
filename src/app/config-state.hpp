/**
 * config-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#ifndef CONFIG_STATE_HPP
#define CONFIG_STATE_HPP

#include <gtkmm/application.h>

#include "glob.hpp"

#include "application.hpp"
#include "config-state-abstr.hpp"
#include "config-window.hpp"
#include "func-utils.hpp"
#include "listener-manager.hpp"
#include "shape-config-window.hpp"
#include "state.hpp"
#include "validator-manager.hpp"

namespace brandy0
{

class ConfigState : public State, public ConfigStateAbstr
{
private:
	ApplicationAbstr *app;
	std::unique_ptr<ConfigWindow> mainWin;
	std::unique_ptr<ShapeConfigWindow> shapeWin;

	void setDefaultParams();
	void setParams(const SimulatorParams&);
	void showWindows();

public:
	ConfigState(ApplicationAbstr *const);
	void activate();
	void activate(const SimulatorParams&);
	void deactivate();

	void submitAll() override;
	void goBackHome() override;
	void openShapeConfig() override;
	void closeAll() override;
};

}

#endif // CONFIG_STATE_HPP
