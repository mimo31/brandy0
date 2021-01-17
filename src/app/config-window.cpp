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

ConfigWindow::ConfigWindow(ConfigStateAbstr *const parent)
	: descriptionLabel("Configure your simulation."),
	rhoEntry("rho (density):"),
	muEntry("mu (viscosity):"),
	x0sel("x = 0 (left)", [this, parent](){
		parent->params->bcx0 = x0sel.getBc();
		parent->validityChangeListeners.invoke();
		}),
	x1sel("x = w (right)", [this, parent](){
		parent->params->bcx1 = x1sel.getBc();
		parent->validityChangeListeners.invoke();
		}),
	y0sel("y = 0 (bottom)", [this, parent](){
		parent->params->bcy0 = y0sel.getBc();
		parent->validityChangeListeners.invoke();
		}),
	y1sel("y = h (top)", [this, parent](){
		parent->params->bcy1 = y1sel.getBc();
		parent->validityChangeListeners.invoke();
		}),
	gridWidthEntry("grid width:"),
	gridHeightEntry("grid height:"),
	dtEntry("dt (time step):"),
	stepsPerFrameEntry("steps per frame:"),
	frameCapacityEntry("frame capacity:"),
	physFrame("physics configuration"),
	compFrame("computation configuration"),
	backHomeButton("back to home"),
	startSimButton("start simulation"),
	parent(parent)
{
	backHomeButton.signal_clicked().connect([parent](){ parent->goBackHome(); });
	startSimButton.signal_clicked().connect([parent](){ parent->submitAll(); });

	rhoEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(rhoEntry, parent->params->rho, SimulatorParams::DEFAULT_RHO, SimulatorParams::MIN_RHO, SimulatorParams::MAX_RHO);
			parent->validityChangeListeners.invoke();
			}
			);
	muEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(muEntry, parent->params->mu, SimulatorParams::DEFAULT_MU, SimulatorParams::MIN_MU, SimulatorParams::MAX_MU);
			parent->validityChangeListeners.invoke();
			}
			);

	gridWidthEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(gridWidthEntry, parent->params->wp, SimulatorParams::DEFAULT_WP, SimulatorParams::MAX_WP);
			parent->validityChangeListeners.invoke();
			parent->dimensionsChangeListeners.invoke();
			}
			);
	gridHeightEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(gridHeightEntry, parent->params->hp, SimulatorParams::DEFAULT_HP, SimulatorParams::MAX_HP);
			parent->validityChangeListeners.invoke();
			parent->dimensionsChangeListeners.invoke();
			}
			);
	stepsPerFrameEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(stepsPerFrameEntry, parent->params->stepsPerFrame, SimulatorParams::DEFAULT_STEPS_PER_FRAME, SimulatorParams::MAX_STEPS_PER_FRAME);
			parent->validityChangeListeners.invoke();
			}
			);
	frameCapacityEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosIntIndicator(frameCapacityEntry, parent->params->frameCapacity, SimulatorParams::DEFAULT_FRAME_CAPACITY, SimulatorParams::MAX_FRAME_CAPACITY);
			parent->validityChangeListeners.invoke();
			}
			);

	dtEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(dtEntry, parent->params->dt, SimulatorParams::DEFAULT_DT, SimulatorParams::MIN_DT, SimulatorParams::MAX_DT);
			parent->validityChangeListeners.invoke();
			}
			);
	
	parent->validityChangeListeners.plug([this, parent]() {
		startSimButton.set_sensitive(parent->inputValidators.isAllValid());
	});

	parent->initListeners.plug([this]() {
		setEntryFields();
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
	rootGrid.attach(physFrame, 0, 1, 1, 1);
	rootGrid.attach(compFrame, 1, 1, 1, 1);
	rootGrid.attach(backHomeButton, 0, 2, 1, 1);
	rootGrid.attach(startSimButton, 1, 2, 1, 1);

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

ConfigWindow::~ConfigWindow()
{
}

void ConfigWindow::setEntryFields()
{
	using std::to_string;
	std::unique_ptr<SimulatorParams>& params = parent->params;

	gridWidthEntry.setText(to_string(params->wp));
	gridHeightEntry.setText(to_string(params->hp));
	rhoEntry.setText(to_string(params->rho));
	muEntry.setText(to_string(params->mu));
	dtEntry.setText(to_string(params->dt));
	stepsPerFrameEntry.setText(to_string(params->stepsPerFrame));
	frameCapacityEntry.setText(to_string(params->frameCapacity));
	x0sel.setBc(params->bcx0);
	x1sel.setBc(params->bcx1);
	y0sel.setBc(params->bcy0);
	y1sel.setBc(params->bcy1);
}

}
