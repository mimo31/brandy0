/**
 * start-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#include "start-state.hpp"

#include <iostream>

namespace brandy0
{

StartState::StartState(Application *const app) : app(app)
{
	win = new StartWindow([=](){
			app->enterNewConfig();
			}, [](std::string s){
			std::cout << "hi -- " << s << std::endl;
			});
}

StartState::~StartState()
{
	delete win;
}

void StartState::activate()
{
	win->show();
	app->gtkapp->add_window(*win);
}

void StartState::deactivate()
{
	win->hide();
}

void StartState::run()
{
	win->show();
	app->gtkapp->run(*win);
}

}
