/**
 * application-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#ifndef APPLICATION_ABSTR_HPP
#define APPLICATION_ABSTR_HPP

#include <gtkmm/window.h>

#include "graphics.hpp"
#include "simulation-params.hpp"
#include "style-manager.hpp"

namespace brandy0
{

/**
 * An abstract class providing the interface for various states to call methods of the parent application
 */
class ApplicationAbstr
{
public:
	StyleManager styleManager;
	GraphicsManager graphicsManager;

	/**
	 * Sets the application state to the configuration state with default simulator
	 */
	virtual void enterNewConfig() = 0;
	virtual void enterExistingConfig(SimulationParams) = 0;
	virtual void enterHome() = 0;
	virtual void enterNewSimulation(SimulationParams) = 0;
	virtual void addWindow(Gtk::Window& win) = 0;
	virtual void run(Gtk::Window& win) = 0;
};

}

#endif // APPLICATION_ABSTR_HPP