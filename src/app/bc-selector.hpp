/**
 * bc-selector.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#ifndef BC_SELECTOR_HPP
#define BC_SELECTOR_HPP

#include <functional>

#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "glob.hpp"

#include "annotated-entry.hpp"
#include "boundary-cond.hpp"
#include "func.hpp"
#include "str.hpp"

namespace brandy0
{

class BCSelector : public Gtk::Frame
{
private:
    Gtk::Grid grid;
    Gtk::Label pressureTypeLabel;
    Gtk::ComboBoxText pressureTypeSelector;
	AnnotatedEntry pEntry;
	Gtk::Label velocityTypeLabel;
	Gtk::ComboBoxText velocityTypeSelector;
    AnnotatedEntry uxEntry;
    AnnotatedEntry uyEntry;

    BoundaryCond bc;

	VoidFunc inputChangeHandler;

    void onPressureTypeChange();
    void onVelocityTypeChange();
    void setEntryFields();

public:
    //BoundaryCondType selectedPressure;

    BCSelector(const str& atDescriptor, const VoidFunc& inputChangeHandler);//const std::function<void()>& validityChangeHandler);
    
    bool hasValidInput() const;
	BoundaryCond getBc() const;
	void setBc(const BoundaryCond& bc);
};

}

#endif // BC_SELECTOR_HPP