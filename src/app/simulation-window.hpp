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
#include <gtkmm/scale.h>
#include <gtkmm/switch.h>

#include "brandy-window.hpp"
#include "display-area.hpp"
#include "scales.hpp"
#include "sim-frame.hpp"
#include "simulation-state-abstr.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

/**
 * Class representing the main simulation window
 */
class SimulationWindow : public BrandyWindow
{
private:
	/// Pointer to the parent (abstract) simulation state
	SimulationStateAbstr *parent;

	/// Display area for visualizing the computed frames
	DisplayArea dArea;
	/// Window's main grid containing the display area and the panel grid
	Gtk::Grid mainGrid;

	/// Grid containing the simulation's control panel
	Gtk::Grid panelGrid;

	/// Button for leaving the simulation state and going back to the configuration state
	Gtk::Button toConfigButton;

	/// Frame for the information and controls regarding the simulation computation
	Gtk::Frame computingFrame;
	/// Grid for the information and controls regarding the simulation computation
	Gtk::Grid computingGrid;
	/// Switch for pausing and resuming the simulation computation (the computation thread)
	Gtk::Switch computingSwitch;
	/// Label with the number of stored frames and the capacity for them
	Gtk::Label frameBufferLabel;
	/// Label with the number of iterations computed as part of the current frame (and their total number required for one frame)
	Gtk::Label curIterLabel;
	/// Label indicating the status of the simulation computation (running / paused / diverged)
	Gtk::Label computingStatusLabel;

	/// Frame for the information and controls regarding the playback of the computed frames
	Gtk::Frame playbackFrame;
	/// Grid for the information and controls regarding the playback of the computed frames
	Gtk::Grid playbackGrid;
	/// Combo box for selecting the playback mode
	Gtk::ComboBoxText playbackModeSelector;
	/// Button for pausing and resuming the playback of the computed frames
	Gtk::Button playPauseButton;
	/// Label with the current time (of the playback) and the total computed time
	Gtk::Label timeLabel;
	/// Label with the speed of the playback
	Gtk::Label playbackSpeedLabel;
	/// Scale for setting (and displaying) the playback time
	MutableTimeScale timeScale;
	/// Scale for setting (and displaying) the playback speed
	SpeedScale playbackSpeedScale;

	/// Frame for the controls of the visuals (visualizations of the computed frames)
	Gtk::Frame viewFrame;
	/// Grid for the controls of the visuals (visualizations of the computed frames)
	Gtk::Grid viewGrid;
	/// Label for the background visual mode selector
	Gtk::Label backDisplayLabel;
	/// Label for the foreground visual mode selector
	Gtk::Label frontDisplayLabel;
	/// Combo box for selecting the background visual mode
	Gtk::ComboBoxText backDisplaySelector;
	/// Combo box for selecting the foreground visual mode 
	Gtk::ComboBoxText frontDisplaySelector;

	/// Button for opening the video export dialog
	Gtk::Button videoExportButton;

	/**
	 * Updates the values (the content) of the control and information widgets based on data in the parent state
	 */
	void updateStats();
	/**
	 * Updates the selected row in the playback mode selector based on the playback mode value in the parent state
	 */
	void updatePlaybackModeSelector();
	/**
	 * Disables (or otherwise adjusts) all widgets that should be disabled when the export dialog is opened
	 */
	void disableWhenExport();
	/**
	 * Enables (or otherwise adjusts) all widgets as they should be when the export dialog is closed
	 */
	void enableWhenNotExport();
	/**
	 * Updates this window by update the contents of the widgets and requesting a redraw of the display are.
	 * This method is the handler for the update event of the parent state.
	 */
	void update();

public:
	SimulationWindow(SimulationStateAbstr *parent);

	/**
	 * Explicitly removes the computing switch from this window before its destruction to prevent segfault due to
	 * https://gitlab.gnome.org/GNOME/gtk/-/issues/3720
	 */
	~SimulationWindow();
};

}

#endif // SIMULATION_WINDOW_HPP
