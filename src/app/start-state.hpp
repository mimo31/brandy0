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

#include "application.hpp"
#include "state.hpp"
#include "start-window.hpp"

namespace brandy0
{

class StartState : public State
{
private:
	ApplicationAbstr* app;
	StartWindow* win;
public:
	StartState(ApplicationAbstr *const);
	void activate();
	void deactivate();
	void run();
	~StartState();
};

}

#endif // START_STATE_HPP
