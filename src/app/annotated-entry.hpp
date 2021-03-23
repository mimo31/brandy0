/**
 * annotated-entry.hpp
 *
 * Author: Viktor Fukala
 * Creted on 2020/12/4
 */
#ifndef ANNOTATED_ENTRY_HPP
#define ANNOTATED_ENTRY_HPP

#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "glob.hpp"

#include "hideable.hpp"
#include "str.hpp"
#include "style-manager.hpp"
#include "validity-indicator.hpp"

namespace brandy0
{

/**
 * A group of widgets used to let the user enter text and show them whether the text is valid.
 * Specifically, this class groups together a label (telling the user what to enter), 
 * a text entry field for the text input, and a validity indicator whether their input is valid.
 * 
 * This class itself does not provide the functionality to determine whether the input is valid;
 * the state of the indicator must (can) be set via public methods of this class.
 */
class AnnotatedEntry
{
private:
	/// The label telling the user what kind of information is expected
	Gtk::Label label;
	/// The entry for the user to enter text
	Gtk::Entry entry;
	/// The indicator to show the user whether their input is valid
	Hideable<ValidityIndicator> indicator;
	/// true iff if this annotated entry is disabled (i.e. visible but greyed-out & not sensitive)
	bool disabled;
public:
	/**
	 * Constructs an annotated entry.
	 * @param label the label to be displayed to the user
	 */
	AnnotatedEntry(const str& label, StyleManager &styleManager);
	/**
	 * Attaches this annotated entry to a Gtk::Grid. The label gets attached to 
	 * (fromX, fromY), the entry to (fromX + 1, fromY), and the indicator to (fromX + 2, fromY)
	 * @param grid the grid to attach to
	 * @param fromX the lowest x coordinate in the grid to use
	 * @param fromY the y coordinate in the grid to use
	 */
	void attachTo(Gtk::Grid& g, const uint32_t fromX, const uint32_t fromY);
	/**
	 * Connects the given callback to the changed signal of the text entry. I.e. the given 
	 * callback will be called whenever the text in the entry changes (the user or an external 
	 * caller changes it).
	 * @param callback the callback
	 */
	void hookInputHandler(const std::function<void()>& callback);
	/**
	 * @return the current text in the entry field
	 */
	str getText() const;
	/**
	 * Sets the text in the entry field.
	 * @param entryText the text to be set to the entry field
	 */
	void setText(const str& entryText);
	/**
	 * Sets the indicator state to @a default.
	 */
	void indicateDefault();
	/**
	 * Sets the indicator state to @a OK.
	 */
	void indicateOk();
	/**
	 * Sets the indicator state to @a invalid.
	 * @param message the error message the indicator should display
	 */
	void indicateInvalid(const str& message);
	/**
	 * @return true iff the state of the indicator is not @a invalid.
	 */
	bool hasValidInput() const;
	/**
	 * Sets this annotated entry to the enabled state. 
	 * Does nothing if already in the enabled state. 
	 * Enabled is the default state after construction.
	 * 
	 * @see disable
	 */
	void enable();
	/**
	 * Sets this annotated entry to the disabled state.
	 * Does nothing if already in the disabled state. 
	 * 
	 * @see enable
	 */
	void disable();
};

}

#endif // ANNOTATED_ENTRY_HPP
