/**
 * start-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef START_STATE_HPP
#define START_STATE_HPP

#include <gtkmm/application.h>

#include "about-window.hpp"
#include "application-abstr.hpp"
#include "state.hpp"
#include "start-state-abstr.hpp"
#include "start-window.hpp"

namespace brandy0
{

/**
 * Class representing the application's start state
 */
class StartState : public State, public StartStateAbstr
{
private:
	/// Pointer to the parent (abstract) application
	ApplicationAbstr *app;
	/// Main window of the start state
	StartWindow mainWin;
	/// About window with information about the project
	AboutWindow aboutWin;
public:
	/**
	 * Constructs a StartState object
	 * @param app pointer to the parent (abstract) application
	 */
	StartState(ApplicationAbstr *app);
	/**
	 * Activates this start state; show this start state's main window
	 */
	void activate();
	void deactivate() override;
	/**
	 * Runs the parent application with this start state's main window
	 */
	void run();

	void goToNewSimulation() override;
	void showAbout() override;
};

}

#endif // START_STATE_HPP
