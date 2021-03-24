/**
 * start-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef START_STATE_HPP
#define START_STATE_HPP

#include <gtkmm/application.h>

#include "glob.hpp"

#include "about-window.hpp"
#include "application-abstr.hpp"
#include "ptr.hpp"
#include "state.hpp"
#include "start-state-abstr.hpp"
#include "start-window.hpp"

namespace brandy0
{

class StartState : public State, public StartStateAbstr
{
private:
	ApplicationAbstr* app;
	uptr<StartWindow> mainWin;
	uptr<AboutWindow> aboutWin;
public:
	StartState(ApplicationAbstr *const);
	void activate();
	void deactivate();
	void run();

	void goToNewSimulation() override;
	void showAbout() override;
};

}

#endif // START_STATE_HPP
