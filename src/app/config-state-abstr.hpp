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

/**
 * The abstract interface for the configuration state for its child windows
 * (@see ApplicationAbstr for more explanation on the *Abstr classes)
 */
class ConfigStateAbstr
{
public:
	/// The pointer to the parent ApplicationAbstr class
	ApplicationAbstr *app;

	/**
	 * Simulation parameters as currently set in the configuration state widgets.
	 * Might be null or otherwise unspecified when the simulation state is not the currently active state.
	 */
	uptr<SimulationParams> params;

	/**
	 * Listeners for the event when the configuration state is activated, the windows have been shown and the parameters set
	 * @see paramsOverwriteListeners for an slightly earlier event (invoked whenever the parameters have been set)
	 */
	ListenerManager initListeners;
	/**
	 * Listeners for the event when the configuration state has been deactivated, but the application continues to run
	 * (meant to allow child windows to stop loops, timers that should only run when the configuration state is active)
	 */
	ListenerManager closeListeners;
	/**
	 * Listeners for the event when some dimensions parameters have been changed (either the physical or the grid dimensions)
	 * (meant to allow the shape editor to refresh the drawn grid at that moment)
	 */
	ListenerManager dimensionsChangeListeners;
	/**
	 * Listeners for the event when any changes have been made to the shape stack in the simulation parameters
	 * (i.e. it has been pushed, popped, or cleared)
	 */
	ListenerManager shapeStackChangeListeners;
	/**
	 * Listeners for the event when the user has changed some parameters such that the validity of the whole configuration could have changed.
	 * (meant to allow updating the indicators of the overall validity of the entered paramters)
	 */
	ListenerManager validityChangeListeners;
	/**
	 * Listeners for the event when the shape configuration window has become shown or has become hidden
	 * (meant to allow updating of widgets that can show/hide said window)
	 */
	ListenerManager shapeConfigOpenedChangeListeners;
	/**
	 * Listeners for the event when the present window has become shown
	 * (meant to allow any initialization of the preset window)
	 */
	ListenerManager presetsOpenListeners;
	/**
	 * Listeners for the event when the simulation parameters have gotten externally changed
	 * (meant to allow updating of widgets that display the current values of simulation parameters)
	 */
	ListenerManager paramsOverwriteListeners;

	/**
	 * Validators of the entirety of the parameter configuration
	 * (it should be possible to submit the parameter configuration iff all of these report true)
	 */
	ValidatorManager inputValidators;

	/// true iff the shape configuration window is currently shown
	bool shapeConfigOpened;
	/// true iff the preset window is currently shown
	bool presetsOpened;

	/**
	 * Submits the current simulation parameters to start a simulation
	 */
	virtual void submitAll() = 0;
	/**
	 * Leaves the configuration state and goes back to the start state
	 */
	virtual void goBackHome() = 0;
	/**
	 * Open the shape configuration window. Does nothing if the said window is already open
	 */
	virtual void openShapeConfig() = 0;
	/**
	 * Closes all open windows except for the main configuration window
	 * (meant to be called when the main configuration window is being closed so that the whole application terminates then)
	 */
	virtual void closeAll() = 0;
	/**
	 * Shows the preset window. Does nothing if the said window is already shown
	 */
	virtual void openPresets() = 0;
	/**
	 * Sets the simulation parameters to a specified preset and hides the preset window
	 * @param preset the simulation parameters from the preset
	 */
	virtual void confirmPreset(const SimulationParams &preset) = 0;

	/**
	 * Constructs the abstract interface of the configuration state
	 * @param app pointer to the parent application object
	 */
	ConfigStateAbstr(ApplicationAbstr *app);
};

}

#endif // CONFIG_STATE_ABSTR_HPP