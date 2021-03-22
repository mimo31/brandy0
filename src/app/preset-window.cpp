/**
 * preset-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#include "preset-window.hpp"

#include "simulation-params-preset.hpp"

namespace brandy0
{

PresetWindow::PresetWindow(ConfigStateAbstr *const parent)
	: parent(parent),
	presetLabel("preset:"),
	warnLabel("warning: By confirming the preset, the current configuration (including that of the obstacles) will get overwritten."),
	confirmButton("confirm preset")
{
	for (const SimulationParamsPreset &preset : SimulationParamsPreset::presets)
		presetSelector.append(preset.name);

	grid.attach(presetLabel, 0, 0);
	grid.attach(presetSelector, 1, 0);
	grid.attach(warnLabel, 0, 1, 2, 1);
	grid.attach(confirmButton, 0, 2, 2, 1);

	add(grid);

	parent->presetsOpenListeners.plug([this]
	{
		presetSelector.set_active(0);
	});

	confirmButton.signal_clicked().connect([this, parent]
	{
		const uint32_t selectedi = presetSelector.get_active_row_number();
		parent->confirmPreset(SimulationParamsPreset::presets[selectedi].params);
	});
	
	show_all_children();
}

}