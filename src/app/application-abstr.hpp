/**
 * application-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/16
 */
#ifndef APPLICATION_ABSTR_HPP
#define APPLICATION_ABSTR_HPP

#include <gtkmm/window.h>

#include "simulator-params.hpp"

namespace brandy0
{

class ApplicationAbstr
{
public:
	virtual void enterNewConfig() = 0;
	virtual void enterExistingConfig(const SimulatorParams) = 0;
	virtual void enterHome() = 0;
	virtual void enterNewSimulation(const SimulatorParams) = 0;
	virtual void addWindow(Gtk::Window& win) = 0;
	virtual void run(Gtk::Window& win) = 0;
};

}

#endif // APPLICATION_ABSTR_HPP