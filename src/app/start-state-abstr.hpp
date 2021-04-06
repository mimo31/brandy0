/**
 * start-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#ifndef START_STATE_ABSTR_HPP
#define START_STATE_ABSTR_HPP

namespace brandy0
{

/**
 * The abstract interface for the start state for its child windows
 * (@see ApplicationAbstr for more explanation on the *Abstr classes)
 */
class StartStateAbstr
{
public:
	/**
	 * Switches the application's state to the configuration state with default simulation parameters.
	 * This should be called when the user wants to start a new simulation.
	 */
	virtual void goToNewSimulation() = 0;
	/**
	 * Shows the about window. If the about window is already shown, this method brings it to the front.
	 */
	virtual void showAbout() = 0;
};

}

#endif // START_STATE_ABSTR_HPP