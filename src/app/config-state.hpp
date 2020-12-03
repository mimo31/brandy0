/**
 * config-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#ifndef CONFIG_STATE_HPP
#define CONFIG_STATE_HPP

#include <gtkmm/application.h>

#include "glob.hpp"

#include "application.hpp"
#include "state.hpp"
#include "config-window.hpp"

namespace brandy0
{

class Application;

class ConfigState : public State
{
private:
	Application* app;
	ConfigWindow* win;
public:
	ConfigState(Application *const);
	void activate();
	void deactivate();
	~ConfigState();
};

}

#endif // CONFIG_STATE_HPP
