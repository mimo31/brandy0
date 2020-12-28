/**
 * bc-selector.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#include "bc-selector.hpp"

#include "conv-utils.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

BCSelector::BCSelector(const std::string& atDescriptor, const std::function<void()>& validityChangeHandler)
    : Gtk::Frame("boundary c. at " + atDescriptor),
    pressureLabel("pressure (type):"),
    uxEntry("velocity.x:"),
    uyEntry("velocity.y:")
{
    pressureSelector.append("Dirichlet");
    pressureSelector.append("von Neumann");
    pressureSelector.set_active(0);
    selectedPressure = PressureBoundaryCond::DIRICHLET;

    grid.attach(pressureLabel, 0, 0);
    grid.attach(pressureSelector, 1, 0);

    uxEntry.attachTo(grid, 0, 1);
    uyEntry.attachTo(grid, 0, 2);

    add(grid);

    pressureSelector.signal_changed().connect(sigc::mem_fun(*this, &BCSelector::onPressureTypeChange));
    uxEntry.hookInputHandler([=]()
    {
        ConvUtils::updateRealIndicator(uxEntry, data->u.x, SimulatorParams::DEFAULT_U, SimulatorParams::MAX_U);
        validityChangeHandler();
    });
    uyEntry.hookInputHandler([=]()
    {
        ConvUtils::updateRealIndicator(uyEntry, data->u.y, SimulatorParams::DEFAULT_U, SimulatorParams::MAX_U);
        validityChangeHandler();
    });
}

void BCSelector::onPressureTypeChange()
{
    data->p = pressureSelector.get_active_row_number() == 0 ? PressureBoundaryCond::DIRICHLET : PressureBoundaryCond::VON_NEUMANN;
}

void BCSelector::setDataLocation(BoundaryCond *const data)
{
    this->data = data;
}

bool BCSelector::hasValidInput() const
{
    return uxEntry.hasValidInput() && uyEntry.hasValidInput();
}

void BCSelector::setEntryFields()
{
    using std::to_string;
    if (data->p == PressureBoundaryCond::DIRICHLET)
        pressureSelector.set_active(0);
    else
        pressureSelector.set_active(1);
    uxEntry.setText(to_string(data->u.x));
    uyEntry.setText(to_string(data->u.y));
}

}