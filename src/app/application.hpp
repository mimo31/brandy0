/**
 * application.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <gtkmm/application.h>

#include "glob.hpp"

#include "config-state.hpp"
#include "state.hpp"
#include "start-state.hpp"

namespace brandy0
{

class StartState;
class ConfigState;

class Application
{
private:
	StartState* startSt;
	ConfigState* configSt;
	State* activeSt;
public:
	Glib::RefPtr<Gtk::Application> gtkapp;
	Application();
	void enterNewConfig();
	void enterHome();
	void run();
	~Application();
};

}

#endif // APPLICATION_HPP
