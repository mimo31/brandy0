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
#include <gtkmm/comboboxtext.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "glob.hpp"

#include "brandy-window.hpp"
#include "display-area.hpp"
#include "sim-frame.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class SimulationWindow : public BrandyWindow
{
private:
	DisplayArea dArea;
	Gtk::Grid mainGrid;
	Gtk::Grid panelGrid;
	Gtk::Button toConfigButton;
	Gtk::Label timeLabel;
	Gtk::Label simulatedToTimeLabel;
	Gtk::Label storedFramesLabel;

	Gtk::Frame viewFrame;
	Gtk::Grid viewGrid;
	Gtk::ComboBoxText backDisplaySelector;
	Gtk::ComboBoxText frontDisplaySelector;

public:
	SimulationWindow(const std::function<void()>& backToConfigCallback);
	void setParams(const SimulatorParams *const);
	void setCurFrame(const SimFrame&);
	void redraw();
	void setTime(const double time, const double of);
	void setSimulatedToTime(const double simulatedTo);
	void setStoredFrames(const uint32_t stored, const uint32_t capacity);
};

}

#endif // SIMULATION_WINDOW_HPP
