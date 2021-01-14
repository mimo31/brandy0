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
#include "shape-config-window.hpp"

namespace brandy0
{

class Application;

class ConfigState : public State
{
private:
	Application *app;
	ConfigWindow *win;
	ShapeConfigWindow *shapeWin;
	SimulatorParams *params;

	void setDefaultParams();
	void setParams(const SimulatorParams&);
	void showWindows();
public:
	ConfigState(Application *const);
	void activate();
	void activate(const SimulatorParams&);
	void deactivate();
	~ConfigState();
};

}

#endif // CONFIG_STATE_HPP
