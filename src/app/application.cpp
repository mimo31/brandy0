/**
 * application.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#include "application.hpp"

namespace brandy0
{

Application::Application()
{
	gtkapp = Gtk::Application::create("");
	startSt = new StartState(this);
	configSt = new ConfigState(this);
}

Application::~Application()
{
	delete startSt;
	delete configSt;
}

void Application::run()
{
	activeSt = startSt;
	startSt->run();
}

void Application::enterNewConfig()
{
	activeSt->deactivate();
	activeSt = configSt;
	configSt->activate();
}

void Application::enterHome()
{
	activeSt->deactivate();
	activeSt = startSt;
	startSt->activate();
}

}
