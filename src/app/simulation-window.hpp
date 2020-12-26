/**
 * simulation-window.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef SIMULATION_WINDOW_HPP
#define SIMULATION_WINDOW_HPP

#include <functional>

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "glob.hpp"

#include "display-area.hpp"
#include "sim-frame.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class SimulationWindow : public Gtk::Window
{
private:
	DisplayArea dArea;
	Gtk::Grid mainGrid;
	Gtk::Button toConfigButton;

public:
	SimulationWindow(const std::function<void()>& backToConfigCallback);
	void setParams(const SimulatorParams *const);
	void setCurFrame(const SimFrame *const);
};

}

#endif // SIMULATION_WINDOW_HPP
