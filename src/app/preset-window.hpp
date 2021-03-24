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

class PresetWindow : public BrandyWindow
{
private:
	ConfigStateAbstr *parent;

	Gtk::Grid grid;
	Gtk::Label presetLabel;
	Gtk::ComboBoxText presetSelector;
	Gtk::Label warnLabel;
	Gtk::Button confirmButton;
public:
	PresetWindow(ConfigStateAbstr *parent);
};

}

#endif // PRESET_WINDOW_HPP