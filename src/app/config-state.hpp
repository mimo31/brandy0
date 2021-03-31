/**
 * config-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#ifndef CONFIG_STATE_HPP
#define CONFIG_STATE_HPP

#include <gtkmm/application.h>

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

/**
 * Class representing the application's configuration state.
 */
class ConfigState : public State, public ConfigStateAbstr
{
private:
	/// The main configuration window
	ConfigWindow mainWin;
	/// The shape configuration window
	ShapeConfigWindow shapeWin;
	/// The preset window (allows to the user to select and set a preset of simulation parameters)
	PresetWindow presetWin;

	/**
	 * Sets the simulation parameters stored in this configuration state and invokes the appropriate event handlers
	 * @param params the simulation parameters to be set
	 */
	void setParams(const SimulationParams &params);
	/**
	 * Shows all the windows that should be shown when the configuration state gets activated
	 */
	void showWindows();

public:
	/**
	 * Constructs a configuration state object
	 * @param app pointer to the parent application object
	 */
	ConfigState(ApplicationAbstr *app);
	/**
	 * Activates the configuration state with default simulation parameters.
	 * Equivalent to activate(SimulationParamsPreset::DefaultParams)
	 */
	void activate();
	/**
	 * Activates the configuration state. In particular, shows the main configuration and the shape configuration window
	 * and sets the simulation parameters to the specified values
	 * @param params the simulation parameters to be set
	 */
	void activate(const SimulationParams &params);

	void deactivate() override;

	void submitAll() override;
	void goBackHome() override;
	void openShapeConfig() override;
	void closeAll() override;
	void openPresets() override;
	void confirmPreset(const SimulationParams &preset) override;
};

}

#endif // CONFIG_STATE_HPP
