/**
 * config-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-state.hpp"

namespace brandy0
{

ConfigState::ConfigState(Application *const app) : app(app)
{
	win = new ConfigWindow([=](){
			app->enterHome();
		});
}

ConfigState::~ConfigState()
{
	delete win;
}

void ConfigState::activate()
{
	app->gtkapp->add_window(*win);
	win->show();
}

void ConfigState::deactivate()
{
	win->hide();
}

}
