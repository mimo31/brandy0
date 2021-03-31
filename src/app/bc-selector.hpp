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

#include "annotated-entry.hpp"
#include "boundary-cond.hpp"
#include "func.hpp"
#include "str.hpp"

namespace brandy0
{

/// A custom widget containing all the fields necessary for configuring one boundary condition of a simulation
class BCSelector : public Gtk::Frame
{
private:
	/// Grid containing all the input widgets and the only direct child of *this
	Gtk::Grid grid;
	/// Label explaining that the pressure type selector is for selecting the type of the pressure b.c.
	Gtk::Label pressureTypeLabel;
	/// ComboBox for selecting the type of the pressure b.c.
	Gtk::ComboBoxText pressureTypeSelector;
	/// AnnotatedEntry for specifying the value of pressure at the boundary provided that the pressure b.c. type is Dirichlet
	AnnotatedEntry pEntry;
	/// Label explaining that the velocity type selector is for selecting the type of the velocity b.c.
	Gtk::Label velocityTypeLabel;
	/// ComboBox for selecting the type of the velocity b.c.
	Gtk::ComboBoxText velocityTypeSelector;
	/**
	 * AnnotatedEntry for specifying the value of the x coordinate of velocity at the boundary
	 * provided that the velocity b.c. type is Dirichlet
	 */
	AnnotatedEntry uxEntry;
	/**
	 * AnnotatedEntry for specifying the value of the y coordinate of velocity at the boundary
	 * provided that the velocity b.c. type is Dirichlet
	 */
	AnnotatedEntry uyEntry;

	/// Representation of the boundary condition as currently displayed by the widgets
	BoundaryCond bc;

	/// A callback passed by whoever created this object that is called whenever the user changes the input
	VoidFunc inputChangeHandler;

	/**
	 * The method that handles signal changed on the pressure type selector
	 */
	void onPressureTypeChange();
	/**
	 * The method that handles signal changed on the velocity type selector
	 */
	void onVelocityTypeChange();
	/**
	 * Sets the input widgets to display values currently stored in this->bc
	 */
	void setEntryFields();

public:
	/**
	 * Constructs the b.c. selector without initializing the values.
	 * @param atDescriptor string to be shown in the frame's label (something like "x = w" is expected here)
	 * @param inputChangeHandler function to be called whenever the represented b. c. configuration changes
	 */
	BCSelector(const str& atDescriptor, const VoidFunc& inputChangeHandler, StyleManager *styleManager);
	
	/**
	 * @return true iff the input currently entered by the user represents a valid b.c. configuration
	 */ 
	bool hasValidInput() const;
	/**
	 * @return the b.c. currently stored
	 */
	BoundaryCond getBc() const;
	/**
	 * Sets the b.c. stored in this widget. Updates the values in the contained accordingly.
	 */
	void setBc(const BoundaryCond& bc);
};

}

#endif // BC_SELECTOR_HPP