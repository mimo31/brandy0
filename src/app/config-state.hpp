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

#include "application-abstr.hpp"
#include "config-state-abstr.hpp"
#include "config-window.hpp"
#include "func.hpp"
#include "listener-manager.hpp"
#include "preset-window.hpp"
#include "ptr.hpp"
#include "shape-config-window.hpp"
#include "state.hpp"
#include "validator-manager.hpp"

namespace brandy0
{

class ConfigState : public State, public ConfigStateAbstr
{
private:
	uptr<ConfigWindow> mainWin;
	uptr<ShapeConfigWindow> shapeWin;
	uptr<PresetWindow> presetWin;

	void setDefaultParams();
	void setParams(const SimulationParams&);
	void showWindows();

public:
	ConfigState(ApplicationAbstr *const);
	void activate();
	void activate(const SimulationParams&);
	void deactivate();

	void submitAll() override;
	void goBackHome() override;
	void openShapeConfig() override;
	void closeAll() override;
	void openPresets() override;
	void confirmPreset(const SimulationParams &preset) override;
};

}

#endif // CONFIG_STATE_HPP
