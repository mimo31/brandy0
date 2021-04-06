/**
 * validity-indicator.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/4
 */
#ifndef VALIDITY_INDICATOR_HPP
#define VALIDITY_INDICATOR_HPP

#include <gtkmm/cssprovider.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "style-manager.hpp"
#include "str.hpp"

namespace brandy0
{

/**
 * Enum for the state of a validity indicator
 */
enum IndicatorState
{
	/// Indicating that the input is valid and not the default value
	OK,
	/// Indicating that the input is the default value (and valid)
	DEFAULT,
	/// Indicating that the input is invalid
	INVALID
};

/**
 * Class for a GTK widget that uses colored text and an icon to indicate whether some input is valid or not
 */
class ValidityIndicator : public Gtk::Grid
{
private:
	/// Label with text for the user
	Gtk::Label message;
	/// Indicator's current icon
	Gtk::Image *icon;
	/// Indicator's current state
	IndicatorState state;
	/**
	 * Deletes the current icon (even if it was the ok icon) and loads and adds the ok icon instead
	 */
	void setOkIcon();
	/**
	 * Deletes the current icon (even if it was the icon indicating invalid input) and loads and adds the icon indicating invalid input
	 */
	void setInvalidIcon();
	/**
	 * Current (color) style of the label with text for the user
	 */
	Glib::RefPtr<Gtk::CssProvider> currentStyle;
	/**
	 * Removes the message label's current style and adds the specified style.
	 * Does nothing if the specified style is the label's current style.
	 * @param style style to add to the label
	 */
	void changeStyle(Glib::RefPtr<Gtk::CssProvider> style);
	/// Pointer to the global style manager
	StyleManager *styleManager;
public:
	/**
	 * Constructs a ValidityIndicator object
	 * @param styleManager style manager to use (typically the global (application's) one)
	 */
	ValidityIndicator(StyleManager *styleManager);
	~ValidityIndicator() override;
	/**
	 * Sets this indicator's state to default
	 */
	void setDefault();
	/**
	 * Sets this indicator's state to ok
	 */
	void setOk();
	/**
	 * Sets this indicator's state to indicating invalid
	 */
	void setInvalid(const str &message);
	/**
	 * @return this indicator's current state
	 */
	IndicatorState getState() const;
};

}

#endif // VALIDITY_INDICATOR_HPP
