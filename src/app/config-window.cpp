/**
 * config-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-window.hpp"

#include <gtkmm/cssprovider.h>

#include "conv-utils.hpp"
#include "simulation-params-preset.hpp"

namespace brandy0
{

ConfigWindow::ConfigWindow(ConfigStateAbstr *const parent)
	: descriptionLabel("Configure your simulation."),
	rhoEntry("rho (density):", parent->app->styleManager),
	muEntry("mu (viscosity):", parent->app->styleManager),
	x0sel("x = 0 (left)", [this, parent](){
		parent->params->bcx0 = x0sel.getBc();
		parent->validityChangeListeners.invoke();
		}, parent->app->styleManager),
	x1sel("x = w (right)", [this, parent](){
		parent->params->bcx1 = x1sel.getBc();
		parent->validityChangeListeners.invoke();
		}, parent->app->styleManager),
	y0sel("y = 0 (bottom)", [this, parent](){
		parent->params->bcy0 = y0sel.getBc();
		parent->validityChangeListeners.invoke();
		}, parent->app->styleManager),
	y1sel("y = h (top)", [this, parent](){
		parent->params->bcy1 = y1sel.getBc();
		parent->validityChangeListeners.invoke();
		}, parent->app->styleManager),
	gridWidthEntry("grid width:", parent->app->styleManager),
	gridHeightEntry("grid height:", parent->app->styleManager),
	dtEntry("dt (time step):", parent->app->styleManager),
	stepsPerFrameEntry("steps per frame:", parent->app->styleManager),
	frameCapacityEntry("frame capacity:", parent->app->styleManager),
	physFrame("physics configuration"),
	compFrame("computation configuration"),
	backHomeButton("back to home"),
	startSimButton("start simulation"),
	presetButton("load a preset"),
	reopenButton("reopen obstacle editor"),
	parent(parent)
{
	backHomeButton.signal_clicked().connect([parent]{ parent->goBackHome(); });
	startSimButton.signal_clicked().connect([parent]{ parent->submitAll(); });
	reopenButton.signal_clicked().connect([parent]{ parent->openShapeConfig(); });
	presetButton.signal_clicked().connect([parent]{ parent->openPresets(); });

	rhoEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(rhoEntry, parent->params->rho, SimulationParamsPreset::DefaultRho, SimulationParamsPreset::MinRho, SimulationParamsPreset::MaxRho);
			parent->validityChangeListeners.invoke();
			}
			);
	muEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(muEntry, parent->params->mu, SimulationParamsPreset::DefaultMu, SimulationParamsPreset::MinMu, SimulationParamsPreset::MaxMu);
			parent->validityChangeListeners.invoke();
			}
			);

	gridWidthEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(gridWidthEntry, parent->params->wp, SimulationParamsPreset::DefaultWp, SimulationParamsPreset::MinWp, SimulationParamsPreset::MaxWp);
			parent->validityChangeListeners.invoke();
			parent->dimensionsChangeListeners.invoke();
			}
			);
	gridHeightEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(gridHeightEntry, parent->params->hp, SimulationParamsPreset::DefaultHp, SimulationParamsPreset::MinHp, SimulationParamsPreset::MaxHp);
			parent->validityChangeListeners.invoke();
			parent->dimensionsChangeListeners.invoke();
			}
			);
	stepsPerFrameEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(stepsPerFrameEntry, parent->params->stepsPerFrame, SimulationParamsPreset::DefaultStepsPerFrame, SimulationParamsPreset::MaxStepsPerFrame);
			parent->validityChangeListeners.invoke();
			}
			);
	frameCapacityEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(frameCapacityEntry, parent->params->frameCapacity, SimulationParamsPreset::DefaultFrameCapacity, SimulationParamsPreset::MinFrameCapacity, SimulationParamsPreset::MaxFrameCapacity);
			parent->validityChangeListeners.invoke();
			}
			);

	dtEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(dtEntry, parent->params->dt, SimulationParamsPreset::DefaultDt, SimulationParamsPreset::MinDt, SimulationParamsPreset::MaxDt);
			parent->validityChangeListeners.invoke();
			}
			);
	
	parent->validityChangeListeners.plug([this, parent]() {
		startSimButton.set_sensitive(parent->inputValidators.isAllValid());
	});

	parent->paramsOverwriteListeners.plug([this]() {
		setEntryFields();
	});

	parent->shapeConfigOpenedChangeListeners.plug([this, parent]()
	{
		if (parent->shapeConfigOpened)
			reopenButton.pseudoHide();
		else
			reopenButton.pseudoShow();
	});

	signal_delete_event().connect([this, parent](GdkEventAny*)
	{
		parent->closeAll();
		return false;
	});

	Glib::RefPtr<Gtk::CssProvider> framePaddingStyle = Gtk::CssProvider::create();
	framePaddingStyle->load_from_data("grid { padding: 5px }");

	physGrid.get_style_context()->add_provider(framePaddingStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);
	compGrid.get_style_context()->add_provider(framePaddingStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

	physGrid.set_column_spacing(5);
	compGrid.set_column_spacing(5);

	rhoEntry.attachTo(physGrid, 0, 0);
	muEntry.attachTo(physGrid, 0, 1);
	physGrid.attach(x0sel, 0, 2, 3, 1);
	physGrid.attach(x1sel, 0, 3, 3, 1);
	physGrid.attach(y0sel, 0, 4, 3, 1);
	physGrid.attach(y1sel, 0, 5, 3, 1);

	physFrame.add(physGrid);

	gridWidthEntry.attachTo(compGrid, 0, 0);
	gridHeightEntry.attachTo(compGrid, 0, 1);
	dtEntry.attachTo(compGrid, 0, 2);
	stepsPerFrameEntry.attachTo(compGrid, 0, 3);
	frameCapacityEntry.attachTo(compGrid, 0, 4);
	
	compFrame.add(compGrid);

	rootGrid.attach(descriptionLabel, 0, 0, 2, 1);
	rootGrid.attach(physFrame, 0, 1, 1, 3);
	rootGrid.attach(compFrame, 1, 1);
	rootGrid.attach(reopenButton, 1, 2);
	rootGrid.attach(presetButton, 1, 3);
	rootGrid.attach(backHomeButton, 0, 4);
	rootGrid.attach(startSimButton, 1, 4);

	add(rootGrid);

	show_all_children();
}

bool ConfigWindow::areInputsValid()
{
	return rhoEntry.hasValidInput()
			&& muEntry.hasValidInput()
			&& gridWidthEntry.hasValidInput()
			&& gridHeightEntry.hasValidInput()
			&& dtEntry.hasValidInput()
			&& stepsPerFrameEntry.hasValidInput()
			&& frameCapacityEntry.hasValidInput()
			&& x0sel.hasValidInput()
			&& x1sel.hasValidInput()
			&& y0sel.hasValidInput()
			&& y1sel.hasValidInput();
}

void ConfigWindow::setEntryFields()
{
	uptr<SimulationParams>& params = parent->params;

	gridWidthEntry.setText(std::to_string(params->wp));
	gridHeightEntry.setText(std::to_string(params->hp));
	rhoEntry.setText(ConvUtils::defaultToString(params->rho));
	muEntry.setText(ConvUtils::defaultToString(params->mu));
	dtEntry.setText(ConvUtils::defaultToString(params->dt));
	stepsPerFrameEntry.setText(std::to_string(params->stepsPerFrame));
	frameCapacityEntry.setText(std::to_string(params->frameCapacity));
	x0sel.setBc(params->bcx0);
	x1sel.setBc(params->bcx1);
	y0sel.setBc(params->bcy0);
	y1sel.setBc(params->bcy1);
}

}
