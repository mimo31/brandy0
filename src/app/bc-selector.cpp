/**
 * bc-selector.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#include "bc-selector.hpp"

#include "conv-utils.hpp"
#include "simulation-params.hpp"
#include "simulation-params-preset.hpp"

namespace brandy0
{

BCSelector::BCSelector(const str& atDescriptor, const VoidFunc& inputChangeHandler, StyleManager &styleManager)
	: Gtk::Frame("boundary c. at " + atDescriptor),
	pressureTypeLabel("pressure (type):"),
	pEntry("pressure:", styleManager),
	velocityTypeLabel("velocity (type):"),
	uxEntry("velocity.x:", styleManager),
	uyEntry("velocity.y:", styleManager),
	inputChangeHandler(inputChangeHandler)
{
	pressureTypeLabel.set_xalign(0);
	velocityTypeLabel.set_xalign(0);
	pressureTypeSelector.append("Dirichlet");
	pressureTypeSelector.append("Neumann");

	grid.attach(pressureTypeLabel, 0, 0);
	grid.attach(pressureTypeSelector, 1, 0);

	pEntry.attachTo(grid, 0, 1);

	velocityTypeSelector.append("Dirichlet");
	velocityTypeSelector.append("Neumann");

	grid.attach(velocityTypeLabel, 0, 2);
	grid.attach(velocityTypeSelector, 1, 2);

	uxEntry.attachTo(grid, 0, 3);
	uyEntry.attachTo(grid, 0, 4);

	add(grid);

	velocityTypeSelector.signal_changed().connect(sigc::mem_fun(*this, &BCSelector::onVelocityTypeChange));
	pressureTypeSelector.signal_changed().connect(sigc::mem_fun(*this, &BCSelector::onPressureTypeChange));
	uxEntry.hookInputHandler([this, inputChangeHandler]
	{
		ConvUtils::updateRealIndicator(uxEntry, bc.u.x, SimulationParamsPreset::DefaultU, SimulationParamsPreset::MaxU);
		inputChangeHandler();
	});
	uyEntry.hookInputHandler([this, inputChangeHandler]
	{
		ConvUtils::updateRealIndicator(uyEntry, bc.u.y, SimulationParamsPreset::DefaultU, SimulationParamsPreset::MaxU);
		inputChangeHandler();
	});
	pEntry.hookInputHandler([this, inputChangeHandler]
	{
		ConvUtils::updateRealIndicator(pEntry, bc.p, SimulationParamsPreset::DefaultP, SimulationParamsPreset::MaxP);
		inputChangeHandler();
	});
}

void BCSelector::onPressureTypeChange()
{
	bc.ptype = pressureTypeSelector.get_active_row_number() == 0 ? BoundaryCondType::Dirichlet : BoundaryCondType::Neumann;
	if (bc.ptype == BoundaryCondType::Dirichlet)
		pEntry.enable();
	else
		pEntry.disable();
	inputChangeHandler();
}

void BCSelector::onVelocityTypeChange()
{
	bc.utype = velocityTypeSelector.get_active_row_number() == 0 ? BoundaryCondType::Dirichlet : BoundaryCondType::Neumann;
	if (bc.utype == BoundaryCondType::Dirichlet)
	{
		uxEntry.enable();
		uyEntry.enable();
	}
	else
	{
		uxEntry.disable();
		uyEntry.disable();
	}
	inputChangeHandler();
}

bool BCSelector::hasValidInput() const
{
	return (bc.ptype == BoundaryCondType::Neumann || pEntry.hasValidInput())
		&& (bc.utype == BoundaryCondType::Neumann || (uxEntry.hasValidInput() && uyEntry.hasValidInput()));
}

void BCSelector::setEntryFields()
{
	if (bc.ptype == BoundaryCondType::Dirichlet)
		pressureTypeSelector.set_active(0);
	else
		pressureTypeSelector.set_active(1);
	if (bc.utype == BoundaryCondType::Dirichlet)
		velocityTypeSelector.set_active(0);
	else
		velocityTypeSelector.set_active(1);
	pEntry.setText(ConvUtils::defaultToString(bc.p));
	uxEntry.setText(ConvUtils::defaultToString(bc.u.x));
	uyEntry.setText(ConvUtils::defaultToString(bc.u.y));
}

BoundaryCond BCSelector::getBc() const
{
	return bc;
}

void BCSelector::setBc(const BoundaryCond& bc)
{
	this->bc = bc;
	setEntryFields();
}

}