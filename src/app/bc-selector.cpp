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

BCSelector::BCSelector(const str& atDescriptor, const VoidFunc& inputChangeHandler)
    : Gtk::Frame("boundary c. at " + atDescriptor),
    pressureLabel("pressure (type):"),
    uxEntry("velocity.x:"),
    uyEntry("velocity.y:"),
	bc(SimulationParamsPreset::defaultBc),
	inputChangeHandler(inputChangeHandler)
{
    pressureSelector.append("Dirichlet");
    pressureSelector.append("Neumann");
    pressureSelector.set_active(0);
    selectedPressure = PressureBoundaryCond::NEUMANN;

    grid.attach(pressureLabel, 0, 0);
    grid.attach(pressureSelector, 1, 0);

    uxEntry.attachTo(grid, 0, 1);
    uyEntry.attachTo(grid, 0, 2);

    add(grid);

    pressureSelector.signal_changed().connect(sigc::mem_fun(*this, &BCSelector::onPressureTypeChange));
    uxEntry.hookInputHandler([=]()
    {
        ConvUtils::updateRealIndicator(uxEntry, bc.u.x, SimulationParamsPreset::DEFAULT_U, SimulationParamsPreset::MAX_U);
        inputChangeHandler();
    });
    uyEntry.hookInputHandler([=]()
    {
        ConvUtils::updateRealIndicator(uyEntry, bc.u.y, SimulationParamsPreset::DEFAULT_U, SimulationParamsPreset::MAX_U);
        inputChangeHandler();
    });
}

void BCSelector::onPressureTypeChange()
{
    bc.p = pressureSelector.get_active_row_number() == 0 ? PressureBoundaryCond::DIRICHLET : PressureBoundaryCond::NEUMANN;
	inputChangeHandler();
}

bool BCSelector::hasValidInput() const
{
    return uxEntry.hasValidInput() && uyEntry.hasValidInput();
}

void BCSelector::setEntryFields()
{
    using std::to_string;
    if (bc.p == PressureBoundaryCond::DIRICHLET)
        pressureSelector.set_active(0);
    else
        pressureSelector.set_active(1);
    uxEntry.setText(to_string(bc.u.x));
    uyEntry.setText(to_string(bc.u.y));
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