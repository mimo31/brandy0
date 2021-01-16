/**
 * config-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-window.hpp"

#include <gtkmm/cssprovider.h>

#include "conv-utils.hpp"

namespace brandy0
{

ConfigWindow::ConfigWindow(const std::function<void()>& backHomeCallback, const std::function<void()>& startSimulationCallback,
	const std::function<void()>& gridSizeChangedCallback)
	: descriptionLabel("Configure your simulation."),
	rhoEntry("rho (density):"),
	muEntry("mu (viscosity):"),
	x0sel("x = 0 (left)", [this](){updateSubmitSensitivity();}),
	x1sel("x = w (right)", [this](){updateSubmitSensitivity();}),
	y0sel("y = 0 (bottom)", [this](){updateSubmitSensitivity();}),
	y1sel("y = h (top)", [this](){updateSubmitSensitivity();}),
	gridWidthEntry("grid width:"),
	gridHeightEntry("grid height:"),
	dtEntry("dt (time step):"),
	stepsPerFrameEntry("steps per frame:"),
	frameCapacityEntry("frame capacity:"),
	physFrame("physics configuration"),
	compFrame("computation configuration"),
	backHomeButton("back to home"),
	startSimButton("start simulation")
{
	backHomeButton.signal_clicked().connect(backHomeCallback);
	startSimButton.signal_clicked().connect(startSimulationCallback);

	rhoEntry.hookInputHandler([this]()
			{
			ConvUtils::updatePosRealIndicator(rhoEntry, params->rho, SimulatorParams::DEFAULT_RHO, SimulatorParams::MIN_RHO, SimulatorParams::MAX_RHO);
			updateSubmitSensitivity();
			}
			);
	muEntry.hookInputHandler([this]()
			{
			ConvUtils::updatePosRealIndicator(muEntry, params->mu, SimulatorParams::DEFAULT_MU, SimulatorParams::MIN_MU, SimulatorParams::MAX_MU);
			updateSubmitSensitivity();
			}
			);

	gridWidthEntry.hookInputHandler([this, gridSizeChangedCallback]()
			{
			ConvUtils::updatePosIntIndicator(gridWidthEntry, params->wp, SimulatorParams::DEFAULT_WP, SimulatorParams::MAX_WP);
			updateSubmitSensitivity();
			gridSizeChangedCallback();
			}
			);
	gridHeightEntry.hookInputHandler([this, gridSizeChangedCallback]()
			{
			ConvUtils::updatePosIntIndicator(gridHeightEntry, params->hp, SimulatorParams::DEFAULT_HP, SimulatorParams::MAX_HP);
			updateSubmitSensitivity();
			gridSizeChangedCallback();
			}
			);
	stepsPerFrameEntry.hookInputHandler([this]()
			{
			ConvUtils::updatePosIntIndicator(stepsPerFrameEntry, params->stepsPerFrame, SimulatorParams::DEFAULT_STEPS_PER_FRAME, SimulatorParams::MAX_STEPS_PER_FRAME);
			updateSubmitSensitivity();
			}
			);
	frameCapacityEntry.hookInputHandler([this]()
			{
			ConvUtils::updatePosIntIndicator(frameCapacityEntry, params->frameCapacity, SimulatorParams::DEFAULT_FRAME_CAPACITY, SimulatorParams::MAX_FRAME_CAPACITY);
			updateSubmitSensitivity();
			}
			);

	dtEntry.hookInputHandler([this]()
			{
			ConvUtils::updatePosRealIndicator(dtEntry, params->dt, SimulatorParams::DEFAULT_DT, SimulatorParams::MIN_DT, SimulatorParams::MAX_DT);
			updateSubmitSensitivity();
			}
			);

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
	rootGrid.attach(physFrame, 0, 1, 1, 1);
	rootGrid.attach(compFrame, 1, 1, 1, 1);
	rootGrid.attach(backHomeButton, 0, 2, 1, 1);
	rootGrid.attach(startSimButton, 1, 2, 1, 1);

	add(rootGrid);

	show_all_children();
}

void ConfigWindow::updateSubmitSensitivity()
{
	startSimButton.set_sensitive(
			rhoEntry.hasValidInput()
			&& muEntry.hasValidInput()
			&& gridWidthEntry.hasValidInput()
			&& gridHeightEntry.hasValidInput()
			&& dtEntry.hasValidInput()
			&& stepsPerFrameEntry.hasValidInput()
			&& frameCapacityEntry.hasValidInput()
			&& x0sel.hasValidInput()
			&& x1sel.hasValidInput()
			&& y0sel.hasValidInput()
			&& y1sel.hasValidInput()
			&& shapeConfigValid);
}

void ConfigWindow::setParamsLocation(SimulatorParams *const params)
{
	this->params = params;
	x0sel.setDataLocation(&(params->bcx0));
	x1sel.setDataLocation(&(params->bcx1));
	y0sel.setDataLocation(&(params->bcy0));
	y1sel.setDataLocation(&(params->bcy1));
}

ConfigWindow::~ConfigWindow()
{
}

void ConfigWindow::setEntryFields()
{
	using std::to_string;
	gridWidthEntry.setText(to_string(params->wp));
	gridHeightEntry.setText(to_string(params->hp));
	rhoEntry.setText(to_string(params->rho));
	muEntry.setText(to_string(params->mu));
	dtEntry.setText(to_string(params->dt));
	stepsPerFrameEntry.setText(to_string(params->stepsPerFrame));
	frameCapacityEntry.setText(to_string(params->frameCapacity));
	x0sel.setEntryFields();
	x1sel.setEntryFields();
	y0sel.setEntryFields();
	y1sel.setEntryFields();
}

void ConfigWindow::setShapeConfigValid(const bool shapeConfigValid)
{
	this->shapeConfigValid = shapeConfigValid;
}

}
