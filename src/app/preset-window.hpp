/**
 * preset-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#ifndef PRESET_WINDOW_HPP
#define PRESET_WINDOW_HPP

#include "gtkmm/button.h"
#include "gtkmm/comboboxtext.h"
#include "gtkmm/grid.h"
#include "gtkmm/label.h"

#include "brandy-window.hpp"
#include "config-state-abstr.hpp"

namespace brandy0
{

/**
 * Class representing the window offering a selection of simulation parameters presets
 */
class PresetWindow : public BrandyWindow
{
private:
	/// Pointer to the parent (abstract) configuration state
	ConfigStateAbstr *parent;

	/// Window's main widget grid
	Gtk::Grid grid;
	/// Label explaining that the combo box shows presets
	Gtk::Label presetLabel;
	/// Combo box with all the presets
	Gtk::ComboBoxText presetSelector;
	/// Label with a warning that selecting (confirming) a presets overwrites all current paramters
	Gtk::Label warnLabel;
	/// Button for confirming preset currently selected in the combo box
	Gtk::Button confirmButton;
public:
	/**
	 * Constructs the preset window object
	 * @param parent pointer to the parent (abstract) configuration state
	 */
	PresetWindow(ConfigStateAbstr *parent);
};

}

#endif // PRESET_WINDOW_HPP